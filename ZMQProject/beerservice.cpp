#include "beerService.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>


BeerService::BeerService()
    : context(1),
    pusher(context, ZMQ_PUSH),
    subscriber(context, ZMQ_SUB),
    beer_brands({ "Leffe","Duvel","Westmalle","Chimay",
                 "Cristal","Karmeliet","Rochefort","Orval", "Kasteelbier" }),
    running(true)
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    subscriber.connect("tcp://benternet.pxl-ea-ict.be:24042");
    subscriber.setsockopt(ZMQ_SUBSCRIBE, "beer?>", 6);
    pusher.connect("tcp://benternet.pxl-ea-ict.be:24041");


    std::thread([this]() {
        while (running) {
            std::this_thread::sleep_for(std::chrono::hours(24));
            if (!running) break;
            resetStats();
            std::cout << "Stats auto-reset (24h)" << std::endl;
        }
    }).detach();
}


BeerService::~BeerService() {
    running = false;
}

void BeerService::run() {
    zmq::message_t msg;
    while (true) {
        subscriber.recv(&msg);
        std::string received(reinterpret_cast<char*>(msg.data()), msg.size());
        processMessage(received);
    }
}

void BeerService::processMessage(const std::string& message) {
    const std::string prefix = "beer?>";
    if (message.rfind(prefix, 0) != 0) {
        handleError("invalid topic");
        return;
    }

    auto payload = message.substr(prefix.size());
    auto [cmd, optName] = parsePayload(payload);

    switch (cmd) {
    case Command::Get:
        if (optName) handleGet(*optName);
        else         handleError("no name provided");
        break;
    case Command::Stats:
        handleStats();
        break;
    case Command::StatsTop:
        handleStatsTop();
        break;
    case Command::Reset:
        resetStats();
        break;
    case Command::Help:
        handleHelp();
        break;
    default:
        handleError("unknown command");
    }
}

std::pair<BeerService::Command, std::optional<std::string>>
BeerService::parsePayload(const std::string& payload) {
    // strip optional '>' at end
    auto p = payload;
    if (!p.empty() && p.back() == '>') p.pop_back();

    if (p.rfind("get>", 0) == 0) {
        auto name = p.substr(4);
        return { Command::Get,
                name.empty() ? std::nullopt : std::optional<std::string>(name) };
    }
    if (p == "stats>top") {
        return { Command::StatsTop, std::nullopt };
    }
    if (p == "stats") {
        return { Command::Stats, std::nullopt };
    }
    if (p == "reset")          return { Command::Reset,    std::nullopt };

    if (p == "help") {
        return { Command::Help, std::nullopt };
    }
    // treat bare payload as name
    if (!p.empty()) {
        return { Command::Get, p };
    }
    return { Command::Invalid, std::nullopt };
}

void BeerService::handleGet(const std::string& name) {
    auto beer = getRandomBeer();
    user_requests[name]++;
    auto reply = "beer!>" + name + ", your beer is: " + beer;
    pusher.send(zmq::message_t(reply.data(), reply.size()));
    std::cout << "Sent: " << reply << std::endl;
}

void BeerService::handleStats() {
    auto reply = getStatsString();
    pusher.send(zmq::message_t(reply.data(), reply.size()));
    std::cout << "Sent: " << reply << std::endl;
}

void BeerService::handleStatsTop() {
    // Thread-safe snapshot
    std::vector<std::pair<std::string,int>> vec;
    {
        std::lock_guard<std::mutex> lock(mtx);
        vec.assign(user_requests.begin(), user_requests.end());
    }
    // Sort descending by count
    std::sort(vec.begin(), vec.end(),
              [](auto &a, auto &b){ return a.second > b.second; });

    // Take up to top 3
    size_t N = std::min<size_t>(3, vec.size());
    std::string out = "beer!>Top:";
    for (size_t i = 0; i < N; ++i) {
        out += " " + vec[i].first + ":" + std::to_string(vec[i].second);
        if (i+1 < N) out += ",";
    }
    pusher.send(zmq::message_t(out.data(), out.size()));
    std::cout << "Sent: " << out << std::endl;
}


void BeerService::handleHelp() {
    auto reply = getHelpString();
    pusher.send(zmq::message_t(reply.data(), reply.size()));
    std::cout << "Sent: " << reply << std::endl;
}

void BeerService::handleError(const std::string& errorMsg) {
    auto reply = "beer!>Error: " + errorMsg;
    pusher.send(zmq::message_t(reply.data(), reply.size()));
    std::cerr << "Error: " << errorMsg << std::endl;
}

std::string BeerService::getRandomBeer() {
    return beer_brands[std::rand() % beer_brands.size()];
}

std::string BeerService::getStatsString() const {
    std::string out = "beer!>Stats:";
    for (auto& [user, cnt] : user_requests) {
        out += " " + user + ":" + std::to_string(cnt) + ",";
    }
    if (!user_requests.empty()) out.pop_back();
    return out;
}

std::string BeerService::getHelpString() const {
    return "beer!>Commands: get>[name], [name], stats, help";
}



void BeerService::resetStats() {
    std::lock_guard<std::mutex> lock(mtx);
    user_requests.clear();
}
