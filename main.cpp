#include <iostream>
#include <string>
#include <vector>
#include <zmq.hpp>


int main( void )
{
    try
    {zmq::context_t context(1);
        zmq::socket_t pusher(context, ZMQ_PUSH);
        zmq::socket_t subscriber(context, ZMQ_SUB);

        // Correcte verbindingen maken
        subscriber.connect("tcp://benternet.pxl-ea-ict.be:24042");
        subscriber.setsockopt(ZMQ_SUBSCRIBE, "beer?>", 6); // Check of de delimiter juist is!

        pusher.connect("tcp://benternet.pxl-ea-ict.be:24041");

        // Biermerken lijst
        std::vector<std::string> beer_brands = {
            "Leffe", "Duvel", "Westmalle", "Chimay",
            "Cristal", "Karmeliet", "Rochefort", "Orval"
        };

        std::srand(std::time(nullptr)); // Random seed

        zmq::message_t msg;

        while (true) {
            subscriber.recv(&msg);
            std::string received_msg(static_cast<char*>(msg.data()), msg.size());
            std::cout << "Received: " << received_msg << std::endl;

            // Check of het bericht begint met "beer!>"
            if (received_msg.rfind("beer?>", 0) == 0) {
                // Kies een random biermerk
                std::string random_beer = "beer!>" +  beer_brands[std::rand() % beer_brands.size()];
                std::cout << "Sending: " << random_beer << std::endl;

                // Verzenden via ZMQ_PUSH
                zmq::message_t reply(random_beer.data(), random_beer.size());
                pusher.send(reply);
            }
        }
    }
    catch (zmq::error_t& ex) {
        std::cerr << "Fout: " << ex.what() << std::endl;
    }

    return 0;
}
