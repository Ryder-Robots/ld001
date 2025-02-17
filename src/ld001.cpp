#include <ld001.hpp>

using namespace rrobot;

int main(int argc, char *argv[]) {
    FatController fatCnt = FatController();
    fatCnt.setMapper(new LdSqu001Mapper());
    fatCnt.run(argc, argv);
    return EXIT_SUCCESS;
}