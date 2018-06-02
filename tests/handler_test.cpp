#include <boost/test/unit_test.hpp>
#include <boost/mpl/assert.hpp>

#include "../src/Handler.h"
#include "../src/Writers.h"
#include "helper.h"

using Commands = std::vector<std::string>;

BOOST_AUTO_TEST_SUITE(test_handler)

    BOOST_AUTO_TEST_CASE(print_commands)
    {
        std::stringbuf out_buffer;
        std::ostream out_stream(&out_buffer);
        auto time = std::make_shared<std::time_t>();

        auto handler = std::make_shared<Handler>();
        auto consoleWriter = std::shared_ptr<Observer>(new ConsoleWriter(out_stream));
        auto fileWriter = std::shared_ptr<Observer>(new FileWriter(time,1));
        consoleWriter->subscribe(handler);
        fileWriter->subscribe(handler);
        handler->setN(2);
        handler->addCommand("cmd1");
        handler->addCommand("cmd2");

        std::ifstream file{name(time)};
        std::stringstream string_stream;
        string_stream << file.rdbuf();

        BOOST_CHECK_EQUAL(out_buffer.str(),"bulk: cmd1, cmd2\n");
        BOOST_CHECK_EQUAL(string_stream.str(),"bulk: cmd1, cmd2");
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(print_blocks)
    {
        std::stringbuf out_buffer;
        std::ostream out_stream(&out_buffer);
        auto time = std::make_shared<std::time_t>();

        auto handler = std::make_shared<Handler>();
        auto consoleWriter = std::shared_ptr<Observer>(new ConsoleWriter(out_stream));
        auto fileWriter = std::shared_ptr<Observer>(new FileWriter(time,1));
        consoleWriter->subscribe(handler);
        fileWriter->subscribe(handler);

        handler->setN(5);
        handler->addCommand("cmd1");
        handler->addCommand("cmd2");
        handler->addCommand("{");

        std::ifstream file{name(time)};
        std::stringstream string_stream;
        string_stream << file.rdbuf();
        file.close();

        BOOST_CHECK_EQUAL(out_buffer.str(),"bulk: cmd1, cmd2\n");
        BOOST_CHECK_EQUAL(string_stream.str(),"bulk: cmd1, cmd2");

        out_buffer.str("");
        string_stream.str("");

        handler->addCommand("cmd3");
        handler->addCommand("cmd4");
        handler->addCommand("}");

        file.open(name(time));
        string_stream << file.rdbuf();
        file.close();

        BOOST_CHECK_EQUAL(out_buffer.str(),"bulk: cmd3, cmd4\n");
        BOOST_CHECK_EQUAL(string_stream.str(),"bulk: cmd3, cmd4");
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(delete_writer)
    {
        std::stringbuf out_buffer;
        std::ostream out_stream(&out_buffer);
        auto time = std::make_shared<std::time_t>();

        auto handler = std::make_shared<Handler>();
        {
            auto fileWriter = std::shared_ptr<Observer>(new FileWriter(time,1));
            fileWriter->subscribe(handler);
        }
        auto consoleWriter = std::shared_ptr<Observer>(new ConsoleWriter(out_stream));
        consoleWriter->subscribe(handler);
        
        handler->setN(1);
        handler->addCommand("cmd1");
        handler->stop();

        std::fstream file{name(time)};
        std::stringstream string_stream;
        string_stream << file.rdbuf();
        file.close();

        BOOST_CHECK_EQUAL(out_buffer.str(),"bulk: cmd1\n");
        BOOST_CHECK_EQUAL(string_stream.str(),"");
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(large_string)
    {
        Handler handler;
        handler.setN(4);
        BOOST_CHECK_THROW(handler.addCommand("123456789012345678901234567890123456789012345678901"),
                            std::exception);
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(error_set_N)
    {
        Handler handler;
        BOOST_CHECK_THROW(handler.addCommand("cmd1"),std::exception);
        handler.setN(5);
        handler.addCommand("cmd1");
        handler.addCommand("cmd2");
        handler.addCommand("cmd3");
        handler.addCommand("cmd4");
        BOOST_CHECK_THROW(handler.setN(3),std::exception);
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(empty_string)
    {
        std::stringbuf out_buffer;
        std::ostream out_stream(&out_buffer);
        auto time = std::make_shared<std::time_t>();

        auto handler = std::make_shared<Handler>();
        auto consoleWriter = std::shared_ptr<Observer>(new ConsoleWriter(out_stream));
        auto fileWriter = std::shared_ptr<Observer>(new FileWriter(time,1));
        consoleWriter->subscribe(handler);
        fileWriter->subscribe(handler);
        
        handler->setN(2);
        handler->addCommand("");
        handler->addCommand("");
        handler->stop();

        std::fstream file;
        file.open(name(time));
        std::stringstream string_stream;
        string_stream << file.rdbuf();
        file.close();

        BOOST_CHECK_EQUAL(out_buffer.str(),"bulk: , \n");
        BOOST_CHECK_EQUAL(out_buffer.str(),"bulk: , \n"); 
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(start_empty_block)
    {
        std::stringbuf out_buffer;
        std::ostream out_stream(&out_buffer);
        auto time = std::make_shared<std::time_t>();

        auto handler = std::make_shared<Handler>();
        auto consoleWriter = std::shared_ptr<Observer>(new ConsoleWriter(out_stream));
        auto fileWriter = std::shared_ptr<Observer>(new FileWriter(time));
        consoleWriter->subscribe(handler);
        fileWriter->subscribe(handler);
        
        handler->setN(2);
        handler->addCommand("{");
        handler->addCommand("}");
        handler->stop();

        std::fstream file;
        file.open(name(time));
        std::stringstream string_stream;
        string_stream << file.rdbuf();
        file.close();

        BOOST_CHECK(out_buffer.str().empty());
        BOOST_CHECK(out_buffer.str().empty()); 
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(empty_block)
    {
        std::stringbuf out_buffer;
        std::ostream out_stream(&out_buffer);

        auto handler = std::make_shared<Handler>();
        auto consoleWriter = std::shared_ptr<Observer>(new ConsoleWriter(out_stream));
        consoleWriter->subscribe(handler);
        
        handler->setN(2);
        handler->addCommand("cmd1");
        handler->addCommand("{");
        handler->addCommand("}");
        handler->addCommand("cmd2");
        handler->stop();

        BOOST_CHECK_EQUAL(out_buffer.str(),"bulk: cmd1\nbulk: cmd2\n");
    }

BOOST_AUTO_TEST_SUITE_END()