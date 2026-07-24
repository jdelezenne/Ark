#include "Ark/Logging/ConsoleTarget.hpp"
#include "Ark/Logging/FormattedConsoleTarget.hpp"
#include "Ark/Logging/JsonFormatter.hpp"
#include "Ark/Logging/Logger.hpp"
#include "Ark/Logging/Macros.hpp"
#include "Ark/Logging/PatternFormatter.hpp"
#include "Ark/Logging/Service.hpp"
#include "Ark/Logging/XmlFormatter.hpp"
#include "Ark/System/EntryPoint.hpp"

namespace Logging = Ark::Logging;

bool arkMain(Ark::Collections::Array<Ark::String> const&)
{
    auto* service = Logging::Service::createService();

    // Phase 1: Pattern
    {
        auto* logger = service->addLogger("Pattern");
        logger->setFormatter(Ark::makeShared<Logging::PatternFormatter>("[%t] %l %c: %m"));
        logger->addTarget(Ark::makeShared<Logging::FormattedConsoleTarget>());

        ARK_LOG(Info, "Pattern phase");
        ARK_LOG(Warning, "Value: {}", 42);
        ARK_LOG(Error, "Something went wrong: {}", "example error");
        service->flushAll();
        service->removeAll();
    }

    // Phase 2: JSON
    {
        auto* jsonLogger = service->addLogger("JSON");
        jsonLogger->setFormatter(Ark::makeShared<Logging::JsonFormatter>(true));
        jsonLogger->addTarget(Ark::makeShared<Logging::ConsoleTarget>());

        ARK_LOG(Info, "JSON phase");
        ARK_LOG(Warning, "Value: {}", 42);
        ARK_LOG(Error, "Something went wrong: {}", "example error");
        service->flushAll();
        service->removeAll();
    }

    // Phase 3: XML
    {
        auto* xmlLogger = service->addLogger("XML");
        xmlLogger->setFormatter(Ark::makeShared<Logging::XmlFormatter>(true));
        xmlLogger->addTarget(Ark::makeShared<Logging::ConsoleTarget>());

        ARK_LOG(Info, "XML phase");
        ARK_LOG(Warning, "Value: {}", 42);
        ARK_LOG(Error, "Something went wrong: {}", "example error");
        service->flushAll();
        service->removeAll();
    }

    return true;
}
