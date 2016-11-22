#include "sfl_viewer.h"

// std
#include <iostream>
#include <exception>

// Boost
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

// Qt
#include <QApplication>

using std::cout;
using std::endl;
using std::cerr;
using std::string;
using std::runtime_error;
using namespace boost::program_options;
using namespace boost::filesystem;


int main(int argc, char* argv[])
{
    // Parse command line arguments
    std::vector<string> inputPaths;
    string landmarksPath, videoPath;
    bool draw_ind;
    try {
        options_description desc("Allowed options");
        desc.add_options()
            ("help", "display the help message")
            ("input,i", value<std::vector<string>>(&inputPaths)->required(),
                "path to video or landmarks (.lms) files")
                ("draw_ind,d", value<bool>(&draw_ind)->default_value(false)->implicit_value(true),
                    "draw landmark indices")
            ;
        variables_map vm;
        store(command_line_parser(argc, argv).options(desc).
            positional(positional_options_description().add("input", -1)).run(), vm);
        if (vm.count("help")) {
            cout << "Usage: cache_face_landmarks [options]" << endl;
            cout << desc << endl;
            exit(0);
        }
        notify(vm);
        if (inputPaths.size() > 2) throw error("Too many input arguments!");
    }
    catch (const error& e) {
        cout << "Error while parsing command-line arguments: " << e.what() << endl;
        cout << "Use --help to display a list of options." << endl;
        exit(1);
    }

    try
    {
        QApplication a(argc, argv);
        sfl::Viewer viewer;
        for (string& inputPath : inputPaths)
            viewer.setInputPath(inputPath);
        viewer.show();

        return a.exec();
    }
    catch (std::exception& e)
    {
        cerr << e.what() << endl;
        return 1;
    }

    return 0;
}