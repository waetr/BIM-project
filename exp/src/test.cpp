#include "argparse.h"
#include "top.h"

int main(int argc, char const *argv[])
{
    auto args = util::argparser("The experiment of BIM.");
    args.set_program_name("exp")
            .add_help_option()
            .add_option("-v", "--verbose", "output verbose message or not")
            .add_option("-l", "--local", "use local value as single spread or not")
            .parse(argc, argv);
    if(args.has_option("--verbose"))
    {
        verbose_flag = 1;
    }
    if(args.has_option("--local"))
    {
        local_mg = 1;
    }
    return 0;
}