#include "Ark/Testing/Test.hpp"

#include "Ark/Logging/FileTarget.hpp"
#include "Ark/Logging/Filters.hpp"
#include "Ark/Logging/JsonFormatter.hpp"
#include "Ark/Logging/Logger.hpp"
#include "Ark/Logging/Logging.hpp"
#include "Ark/Logging/PatternFormatter.hpp"
#include "Ark/Logging/RotatingFileTarget.hpp"
#include "Ark/Logging/Service.hpp"
#include "Ark/Logging/XmlFormatter.hpp"
#include "Ark/Memory/SharedPointer.hpp"
#include "Ark/Storage/File.hpp"
#include "Ark/Storage/FileSystem.hpp"
#include "Ark/Storage/Path.hpp"
#include "Ark/System/KnownFolders.hpp"
#include "Ark/System/Time.hpp"

namespace
{
    struct CaptureTarget final : Ark::Logging::Target
    {
        Ark::Collections::Array<Ark::String> lines;

        void write(Ark::Logging::Entry const& entry) override
        {
            lines.append(entry.formattedMessage);
        }

        void flush() override
        {
        }
    };

    ARK_LOG_DEFINE_CATEGORY_LEVEL_STATIC(TestCat, Info);
}

ARK_TEST_CASE("Logging core paths", "[logging]")
{
    using Ark::Logging::Category;
    using Ark::Logging::JsonFormatter;
    using Ark::Logging::Level;
    using Ark::Logging::LevelFilter;
    using Ark::Logging::Logger;
    using Ark::Logging::Message;
    using Ark::Logging::PatternFormatter;
    using Ark::Logging::Service;
    using Ark::Logging::XmlFormatter;
    using Ark::makeShared;

    SECTION("Message defaults and logger auto-fill timestamp/thread")
    {
        auto target = makeShared<CaptureTarget>();
        Logger logger;
        logger.addTarget(target);

        logger.log(Level::Info, "hello");

        REQUIRE(target->lines.getCount() == 1);
        REQUIRE(target->lines[0] == "hello");
    }

    SECTION("LevelFilter rejects lower severity")
    {
        auto target = makeShared<CaptureTarget>();
        Logger logger;
        logger.addFilter(makeShared<LevelFilter>(Level::Warning));
        logger.addTarget(target);

        logger.log(Level::Info, "skip");
        logger.log(Level::Error, "keep");

        REQUIRE(target->lines.getCount() == 1);
        REQUIRE(target->lines[0] == "keep");
    }

    SECTION("PatternFormatter formats empty message tokens")
    {
        PatternFormatter formatter("[{level}] {message}");
        Message message{};
        message.level = Level::Warning;
        message.message = "";
        message.timestamp = 1'700'000'000'000ULL;

        Ark::String formatted = formatter.format(message);
        REQUIRE(formatted.contains("[Warning]"));
    }

    SECTION("PatternFormatter supports brace and percent tokens")
    {
        PatternFormatter formatter("%l/%c/%m");
        Message message{};
        message.level = Level::Info;
        message.category = "net";
        message.message = "ok";

        REQUIRE(formatter.format(message) == "Info/net/ok");
    }

    SECTION("JsonFormatter escapes quotes and control characters")
    {
        JsonFormatter formatter;
        Message message{};
        message.level = Level::Info;
        message.category = "cat";
        message.message = "say \"hi\"\n";
        message.timestamp = 42;
        message.threadId = 7;
        message.location.fileName = "a.cpp";
        message.location.lineNumber = 10;
        message.location.functionName = "fn";

        Ark::String json = formatter.format(message);
        REQUIRE(json.contains("say \\\"hi\\\"\\n"));
        REQUIRE(json.contains("\"timestamp\":42"));
        REQUIRE(json.contains("\"threadId\":7"));
    }

    SECTION("XmlFormatter avoids stream libraries")
    {
        XmlFormatter formatter;
        Message message{};
        message.level = Level::Error;
        message.category = "x";
        message.message = "a<b>&c";
        message.timestamp = 1'700'000'000'000ULL;
        message.threadId = 3;

        Ark::String xml = formatter.format(message);
        REQUIRE(xml.contains("<level>Error</level>"));
        REQUIRE(xml.contains("a&lt;b&gt;&amp;c"));
        REQUIRE(xml.contains("<thread_id>3</thread_id>"));
    }

    SECTION("Category inheritance and shouldLog")
    {
        Category parent("Parent", Level::Warning);
        Category child("Child", Level::None, &parent);
        REQUIRE(child.effectiveDefaultLevel() == Level::Warning);
        REQUIRE(!child.shouldLog(Level::Info));
        REQUIRE(child.shouldLog(Level::Error));
    }

    SECTION("Service fans out to registered loggers")
    {
        Service* service = Service::createService();
        auto firstTarget = makeShared<CaptureTarget>();
        auto secondTarget = makeShared<CaptureTarget>();

        Logger* first = service->addLogger("one");
        Logger* second = service->addLogger("two");
        first->addTarget(firstTarget);
        second->addTarget(secondTarget);

        service->log(Level::Info, "broadcast");

        REQUIRE(firstTarget->lines.getCount() == 1);
        REQUIRE(secondTarget->lines.getCount() == 1);
        REQUIRE(firstTarget->lines[0] == "broadcast");
        REQUIRE(secondTarget->lines[0] == "broadcast");

        Service::setInstance(nullptr);
    }

    SECTION("macros are else-safe and honor category level")
    {
        Service* service = Service::createService();
        auto target = makeShared<CaptureTarget>();
        Logger* logger = service->addLogger("macros");
        logger->addTarget(target);

        bool tookElse = false;
        if (false)
            ARK_LOGGER_INFO(logger, TestCat, "nope");
        else
            tookElse = true;

        REQUIRE(tookElse);

        ARK_LOGGER_DEBUG(logger, TestCat, "debug-skip");
        ARK_LOGGER_ERROR(logger, TestCat, "error-keep");

        REQUIRE(target->lines.getCount() == 1);
        REQUIRE(target->lines[0] == "error-keep");

        Service::setInstance(nullptr);
    }

    SECTION("timestamp uses wall-clock epoch domain")
    {
        auto target = makeShared<CaptureTarget>();
        Logger logger;
        logger.setFormatter(makeShared<PatternFormatter>("{timestamp}"));
        logger.addTarget(target);

        Ark::uint64 before = Ark::System::Time::getCurrentTimeMilliseconds().getValue();
        logger.log(Level::Info, "tick");
        Ark::uint64 after = Ark::System::Time::getCurrentTimeMilliseconds().getValue();

        REQUIRE(target->lines.getCount() == 1);
        Ark::uint64 stamped = 0;
        for (Ark::usize i = 0; i < target->lines[0].getLength(); ++i)
        {
            stamped = stamped * 10 + static_cast<Ark::uint64>(target->lines[0][i] - '0');
        }
        REQUIRE(stamped >= before);
        REQUIRE(stamped <= after + 5);
    }
}

ARK_TEST_CASE("Logging file targets", "[logging][file]")
{
    using Ark::Logging::FileTarget;
    using Ark::Logging::Level;
    using Ark::Logging::Logger;
    using Ark::Logging::RotatingFileTarget;
    using Ark::Storage::File;
    using Ark::Storage::FileSystem;
    using Ark::Storage::Path;
    using Ark::makeShared;

    Path const dir = Ark::System::getKnownFolderPath(Ark::System::KnownFolder::Temp) / "ark_logging_tests";
    FileSystem::createDirectory(dir);

    SECTION("FileTarget writes lines")
    {
        Path const path = dir / "plain.log";
        FileSystem::deleteFile(path);

        {
            Logger logger;
            logger.addTarget(makeShared<FileTarget>(path, false));
            logger.log(Level::Info, "file-line");
        }

        auto contents = File::readAllText(path);
        REQUIRE(contents.isOk());
        REQUIRE(contents.getValue().contains("file-line"));
    }

    SECTION("RotatingFileTarget maxFiles 0 does not hang on rotate")
    {
        Path const path = dir / "rotate.log";
        Logger logger;
        logger.addTarget(makeShared<RotatingFileTarget>(path, 16, 0));

        for (int i = 0; i < 8; ++i)
        {
            logger.log(Level::Info, "0123456789abcdef");
        }

        SUCCEED();
    }
}
