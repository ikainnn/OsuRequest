import sys
from os import system


def parse_command_arguments(arguments):
    extraArguments = ""

    if len(arguments): print("Parsing extra arguments:")

    for argument in arguments:
        extraArguments.__add__(" " + argument)
        print("\t" + argument + "\n")

    return extraArguments

def main(arguments):
    arguments.pop(0) # ignores the first argument (aka executable name)

    extraArguments = parse_command_arguments(arguments)
    configurationCommand = format("cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=true {} -G \"Ninja\" -S . -B Build/", extraArguments)

    system(configurationCommand)


if __name__ == "__main__":
    main(sys.argv)
