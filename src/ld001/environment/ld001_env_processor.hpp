#ifndef LD001_ENV_PROCESSOR_HPP
#define LD001_ENV_PROCESSOR_HPP

#include <vector>
#include <boost/program_options.hpp>
#include <nlohmann/json.hpp>
#include <dlib/logger.h>
#include <fatcnt/environment/environment.hpp>
#include <fatcnt/exceptions/exceptions.hpp>
#include <ld001/events/catagorizer/ldsqu001mapper.hpp>

using namespace std;
using json = nlohmann::json;

namespace rrobot {

    /**
     * @class Ld001EnvProcessor
     * @brief
     * Implements interface 
     */
    class Ld001EnvProcessor {
        public:

            /**
             * @fn createEnvironment
             * @brief
             * Creates environment, 
             *     - environment variables override manifest settings
             *     - command line options override environment variables
             */
            static Environment createEnvironment(json manifest);
            static Environment* createEnvironmentRef(json manifest);
        
        private:

            // Common vairables that are used ditrectly by fat controller are defined here.
            static HwModel  createHwModel(json manifest);
            static RrSerial createMc(json manifest);
            static Queues   createQueues(json manifest);
            static Server   createServer(json server);
            static Logging  createLogging(json logging);
            static EaiTrainingData createAiTrainingData(json ai_training);

            static bool verify(json manifest, std::vector<string> keys, string container);

    };
}

#endif // LD001_ENV_PROCESSOR_HPP