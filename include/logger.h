#define LOG(level, msg, args...) \
        fprintf(stderr, "%s %s:%d: " msg "\n", level, __FILE__,__LINE__,##args)
#define LOG_DEBUG(msg, args...) LOG("DEBUG", msg,##args)
#define LOG_ERROR(msg, args...) LOG("ERROR", msg,##args)
#define LOG_WARNING(msg, args...) LOG("WARNING", msg,##args)
#define LOG_INFO(msg, args...) LOG("INFO", msg, ##args)

#define LOG_IF_ERROR(condition, message, args...) \
({                                                \
        if (condition) {                          \
                LOG_ERROR(message,##args);      \
        }                                         \
})
