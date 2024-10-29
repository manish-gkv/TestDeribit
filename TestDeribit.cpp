#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <map>
#include <memory>
#include <openssl/ssl.h>

using namespace std;
using json = nlohmann::json;
using websocketpp::connection_hdl;
typedef websocketpp::client<websocketpp::config::asio_tls_client> client;

// WebSocket client
class TestTrading {
public:
    TestTrading() {
        
        ws_client.set_access_channels(websocketpp::log::alevel::all);
        ws_client.clear_access_channels(websocketpp::log::alevel::frame_payload);
        ws_client.init_asio();
        
        // Set TLS init handler
        ws_client.set_tls_init_handler(bind(&TestTrading::on_tls_init, this, placeholders::_1));

        ws_client.set_open_handler(bind(&TestTrading::on_open, this, placeholders::_1)); 
        ws_client.set_close_handler(bind(&TestTrading::on_close, this, placeholders::_1));
        ws_client.set_fail_handler(bind(&TestTrading::on_fail, this, placeholders::_1));
        ws_client.set_message_handler(bind(&TestTrading::on_message, this, placeholders::_1, placeholders::_2));
    }

    void connect(const string& uri) {
        websocketpp::lib::error_code ec;
        client::connection_ptr con = ws_client.get_connection(uri, ec);

        if (ec) {
            cout << "Could not create connection because: " << ec.message() << endl;
            return;
        }

        ws_client.connect(con);
        ws_client.run();
    }

    void disconnect() {
        ws_client.stop();
    }

    // Send a message over the WebSocket
    void send_message(const string& message) {
        websocketpp::lib::error_code ec;
        ws_client.send(connection, message, websocketpp::frame::opcode::text, ec);

        if (ec) {
             cout << "Send failed because: " << ec.message() << endl;
        }
    }

	void placeOrder(const string& action,
                const string& instrument_name,
                double amount = 0,
                double contracts = 0,
                const string& type = "market",
                const string& label = "",
                double price = 0,
                const string& time_in_force = "good_til_cancelled",
                double max_show = 0,
                bool post_only = false,
                bool reduce_only = false,
                bool reject_post_only = false,
                double trigger_price = 0,
                double trigger_offset = 0,
                const string& trigger = "",
                const string& advanced = "",
                bool mmp = false,
                int64_t valid_until = 0,
                const string& linked_order_type = "",
                const string& trigger_fill_condition = "first_hit",
                json otoco_config = json::array()) {

    

    json arguments;
    arguments["instrument_name"] = instrument_name;

    if (amount > 0) arguments["amount"] = amount;
    if (contracts > 0) arguments["contracts"] = contracts;
    if (!type.empty()) arguments["type"] = type;
    if (!label.empty()) arguments["label"] = label;
    if (price > 0) arguments["price"] = price;
    if (!time_in_force.empty()) arguments["time_in_force"] = time_in_force;
    if (max_show > 0) arguments["max_show"] = max_show;
    if (post_only) arguments["post_only"] = post_only;
    if (reduce_only) arguments["reduce_only"] = reduce_only;
    if (reject_post_only) arguments["reject_post_only"] = reject_post_only;
    if (trigger_price > 0) arguments["trigger_price"] = trigger_price;
    if (trigger_offset > 0) arguments["trigger_offset"] = trigger_offset;
    if (!trigger.empty()) arguments["trigger"] = trigger;
    if (!advanced.empty()) arguments["advanced"] = advanced;
    if (mmp) arguments["mmp"] = mmp;
    if (valid_until > 0) arguments["valid_until"] = valid_until;
    if (!linked_order_type.empty()) arguments["linked_order_type"] = linked_order_type;
    if (!trigger_fill_condition.empty()) arguments["trigger_fill_condition"] = trigger_fill_condition;
    if (!otoco_config.empty()) arguments["otoco_config"] = otoco_config;

    json order = {
        {"method", "private/"+ action}, // Set the action based on the parameter
        {"params", arguments}
    };
    cout<<order<<endl;
    send_message(order.dump());
}


    void cancelOrder(string orderId) {
        json cancel = {
            {"method", "private/cancel"},
        	{"params", {
            	{"order_id", orderId}
        	}},
        	{"jsonrpc", "2.0"},
        	{"id", 10}
        };
        cout<<cancel<<endl;
        send_message(cancel.dump());
    }


   void modifyOrder(const string& order_id,
                  double amount = 0,
                  double contracts = 0,
                  double price = 0,
                  bool post_only = false,
                  bool reduce_only = false,
                  bool reject_post_only = false,
                  const string& advanced = "",
                  double trigger_price = 0,
                  double trigger_offset = 0,
                  bool mmp = false,
                  int64_t valid_until = 0) {

    json arguments;
    arguments["order_id"] = order_id;

    if (amount > 0) arguments["amount"] = amount;
    if (contracts > 0) arguments["contracts"] = contracts;
    if (price > 0) arguments["price"] = price;
    if (post_only) arguments["post_only"] = post_only;
    if (reduce_only) arguments["reduce_only"] = reduce_only;
    if (reject_post_only) arguments["reject_post_only"] = reject_post_only;
    if (!advanced.empty()) arguments["advanced"] = advanced;
    if (trigger_price > 0) arguments["trigger_price"] = trigger_price;
    if (trigger_offset > 0) arguments["trigger_offset"] = trigger_offset;
    if (mmp) arguments["mmp"] = mmp;
    if (valid_until > 0) arguments["valid_until"] = valid_until;

    json modify = {
        {"method", "private/edit"},
        {"params", arguments}
    };
    cout<<modify<<endl;
    send_message(modify.dump());
}
 

    void getOrderBook(const string& instrument, int depth=100) {
        json orderbook = {
        	{"method", "public/get_order_book"},
        	{"params", {
            	{"instrument_name", instrument},
            	{"depth", depth}
        	}},
        	{"jsonrpc", "2.0"},
        	{"id", 12}    
        };
        cout<<orderbook<<endl;
        send_message(orderbook.dump());
    }

    void viewPositions(string currency) {
        json positions = {
     		{"method", "private/get_positions"},
        	{"params", {
            	{"currency", currency}
        	}},
        	{"jsonrpc", "2.0"},
        	{"id", 69}
        };
        cout<<positions<<endl;
        send_message(positions.dump());
    }
    void test(){
        json sample = {
          {"method", "public/test"} 
        };
		send_message(sample.dump());
    }

    void auth(){
        json Auth = {
			{"id", 69},
        	{"method", "public/auth"},
        	{"params", {
            	{"grant_type", "client_credentials"},
            	{"client_id", deribitClientID},
            	{"client_secret", deribitClientSecret}
        	}},
        	{"jsonrpc", "2.0"}
		};
		send_message(Auth.dump());
    }
private:
    client ws_client;
    connection_hdl connection;
    string deribitClientID = "z-_YJB-j";
    string deribitClientSecret = "gfumdXXDjK1rO0HTfFGnbDiw9IUZxCLjx-L8OChZFGA";
	
	enum authType{
		buy, sell, cancel, orderBook, postion
	};
	
    // TLS initialization
    shared_ptr<websocketpp::lib::asio::ssl::context> on_tls_init(connection_hdl hdl) {
        auto ctx = make_shared<websocketpp::lib::asio::ssl::context>(websocketpp::lib::asio::ssl::context::tlsv12);

        try {
            ctx->set_options(websocketpp::lib::asio::ssl::context::default_workarounds |
                             websocketpp::lib::asio::ssl::context::no_sslv2 |
                             websocketpp::lib::asio::ssl::context::no_sslv3 |
                             websocketpp::lib::asio::ssl::context::single_dh_use);
            
        } catch (std::exception& e) {
            cerr << "Error in context pointer: " << e.what() << endl;
        }

        return ctx;
    }

    void on_open(connection_hdl hdl) {
        connection = hdl;
        cout << "Connected!" << endl;
    }

    void on_close(connection_hdl hdl) {
        cout << "Connection Closed!" << endl;
    }

    void on_fail(connection_hdl hdl) {
        cout << "Connection Failed!" << endl;
    }

    void on_message(connection_hdl hdl, client::message_ptr msg) {
    json received_json =json::parse(msg->get_payload());

     
	cout<<endl << "Received: " << received_json.dump() << endl<<endl;
    }
    
};

int main() {
    TestTrading tradingWs;
    string uri = "wss://test.deribit.com/ws/api/v2/";
    
    thread connectionThread([&]() {
        tradingWs.connect(uri);
    });
    this_thread::sleep_for(chrono::seconds(2)); // Wait for connection 
   
	thread authThread(&TestTrading::auth, &tradingWs);
   
    authThread.join();

    
    thread buying([&](){
        tradingWs.placeOrder("buy", "BTC-PERPETUAL", 10);        
    });
    
    thread selling([&](){
        tradingWs.placeOrder("sell", "BTC-PERPETUAL", 7);
    });

	thread modifying([&](){
		tradingWs.modifyOrder("29239379539", 4);
    });

    thread canceling([&](){
        tradingWs.cancelOrder("29239382500");
    });

    thread orderBook([&](){
        tradingWs.getOrderBook("BTC-PERPETUAL");
    });

    thread position([&]{
        tradingWs.viewPositions( "ETH");    
    });

    position.join();
    orderBook.join();
    canceling.join();
	modifying.join();
    selling.join();
    buying.join();
	
	this_thread::sleep_for(chrono::seconds(5));	
	//closing connection    
	tradingWs.disconnect();
	this_thread::sleep_for(chrono::seconds(2));

    connectionThread.join();
    return 0;
}

