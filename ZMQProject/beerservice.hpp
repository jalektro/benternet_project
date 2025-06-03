#ifndef BEERSERVICE_HPP
#define BEERSERVICE_HPP

#include <string>
#include <vector>
#include <map>
#include <optional>
#include <zmq.hpp>
#include <thread>
#include <mutex>
#include <chrono>

class BeerService {
public:
    BeerService();
    ~BeerService();
    void run();

private:
    enum class Command {
        Get,
        Stats,
        StatsTop,
        Reset,
        Help,
        Invalid
    };

    zmq::context_t context;
    zmq::socket_t pusher;
    zmq::socket_t subscriber;

    std::vector<std::string> beer_brands;
    std::map<std::string, int> user_requests;

    std::mutex mtx;
    bool running;

    void processMessage(const std::string& message);
    // RETURNS: Command + optional user name (for Get), empty for Stats/Help/Invalid
    std::pair<Command, std::optional<std::string>>
    parsePayload(const std::string& payload);

    void handleGet(const std::string& name);
    void handleStats();
    void handleStatsTop();
    void handleHelp();
    void handleError(const std::string& errorMsg);

    void resetStats() ;

    std::string getRandomBeer();
    std::string getStatsString() const;
    std::string getHelpString() const;
};

#endif // BEERSERVICE_HPP
