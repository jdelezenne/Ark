#include "Ark/Logging/FormattedConsoleTarget.hpp"
#include "Ark/Logging/Logger.hpp"
#include "Ark/Logging/Macros.hpp"
#include "Ark/Logging/PatternFormatter.hpp"
#include "Ark/Logging/Service.hpp"
#include "Ark/System/EntryPoint.hpp"

// Declare categories (header-like)
ARK_LOG_DECLARE_CATEGORY(Core);
ARK_LOG_DECLARE_CATEGORY(Render);
ARK_LOG_DECLARE_CATEGORY(IO);
ARK_LOG_DECLARE_CATEGORY(RenderPass);

// Define categories (one TU)
ARK_LOG_DEFINE_CATEGORY_LEVEL(Core, Info)
ARK_LOG_DEFINE_CATEGORY_LEVEL(Render, Debug)
ARK_LOG_DEFINE_CATEGORY_LEVEL(IO, Warning)
ARK_LOG_DEFINE_CATEGORY_LEVEL_PARENT(RenderPass, Debug, Render)

namespace Logging = Ark::Logging;

bool arkMain(Ark::Collections::Array<Ark::String> const&)
{
    auto* service = Logging::Service::createService();

    auto* logger = service->addLogger("Categories");
    logger->setFormatter(Ark::makeShared<Logging::PatternFormatter>("[%t] %l %c: %m"));
    logger->addTarget(Ark::makeShared<Logging::FormattedConsoleTarget>());

    ARK_LOG_INFO_FORMAT(Core, "Engine starting (version: {}.{})", 1, 0);
    ARK_LOG_DEBUG_FORMAT(Render, "Begin frame {}", 1);
    ARK_LOG_DEBUG_FORMAT(RenderPass, "Pass '{}' setup", "GBuffer");
    ARK_LOG_WARNING_FORMAT(IO, "Config file '{}' missing, using defaults", "settings.ini");
    ARK_LOG_ERROR_FORMAT(Core, "Subsystem '{}' failed to initialize", "Audio");

    service->flushAll();
    service->removeAll();
    return true;
}
