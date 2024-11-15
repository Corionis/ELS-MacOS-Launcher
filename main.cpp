#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fstream>
#include <iostream>
#include <unistd.h>

/**
 * MacOS Launcher for ELS, https://github.com/Corionis/ELS
 *
 */

int main(int argc, char *argv[]) {
    char cwd[PATH_MAX];
    bool isLogging = false;
    std::ofstream log;

    // is launcher logging enabled?
    for (int i = 1; i < argc; ++i)
    {
        std::string search(argv[i]);
        int p = search.find("--launcher-log");
        if (p >= 0)
        {
            isLogging = true;
        }
    }

    if (isLogging)
        log.open("/Users/trh/Work/corionis/ELS/mock/output/ELS-MacOS-Launcher.log");

    getcwd(cwd, sizeof(cwd));
    if (isLogging)
        log << "cwd! " << cwd << std::endl;

    // find the root from argv[0] by removing /ELS-Navigator.app/Contents/MacOS/ELS-Navigator
    if (argc > 0)
    {
        if (isLogging)
            log << "arg0: " << argv[0] <<std::endl;

        int count = 0;
        int pos = 0;
        for (size_t i = strlen(argv[0]); i > 0; --i)
        {
            if (argv[0][i] == '/')
            {
                ++count;
                if (count == 4)
                {
                    pos = i;
                    break;
                }
            }
        }
        // clear cwd
        for (int i = 0; i < sizeof(cwd); ++i)
        {
            cwd[i] = '\0';
        }
        std::strncpy(cwd, &argv[0][0], pos);
    }

    if (isLogging)
        log << "cwd! " << cwd << std::endl;

    // detect -C [configuration directory] argument
    bool dashC = true;
    for (int i = 1; i < argc; ++i)
    {
        std::string search(argv[i]);
        int p = search.find("-C");
        if (p >= 0)
        {
            dashC = false;

            // clear cwd
            for (int i = 0; i < sizeof(cwd); ++i)
            {
                cwd[i] = '\0';
            }

            // copy working directory argument
            if (i + 1 < argc)
                strcpy(cwd, argv[i + 1]);
            else
            {
                if (isLogging)
                    log << "Exception: -C option requires a configuration directory argument" << std::endl;
                return 1;
            }

            // make sure the directory exists
            std::filesystem::path directoryPath(cwd);
            if (std::filesystem::exists(directoryPath))
            {
                if (!std::filesystem::is_directory(directoryPath))
                {
                    if (isLogging)
                        log << "Exception: -C \"" << cwd << "\" exists but is not a directory" << std::endl;
                    return 1;
                }
            }
            else
            {
                mkdir(cwd, 0x755);
                chdir(cwd);
            }
            break;
        }
    }

    chdir(cwd);

    // double check working directory
    char wd[PATH_MAX];
    getcwd(wd, sizeof(cwd));
    if (isLogging)
        log << "wd: " << wd << std::endl;

    // the java command to execute
    std::string command(cwd);;
    command += "/rt/Contents/Home/bin/java";

    // setup arguments
    int sz = dashC ? argc + 5 : argc + 3;
    if (isLogging)
        sz = sz - 1; // --launcher-log is for the launcher only and skipped for the Navigator
    char *arguments[sz];
    int index = 0;

    // argv[0]
    arguments[index++] = command.data();

    // override the java icon
    std::string arg1 = "-Xdock:icon=bin/els-logo-98px.icns";
    arguments[index++] = arg1.data();

    // the Jar
    std::string arg2 = "-jar";
    arguments[index++] = arg2.data();

    std::string arg3(cwd);
    arg3 += "/bin/ELS.jar";
    arguments[index++] = arg3.data();

    // add -C [working-directory] if not specified
    if (dashC)
    {
        std::string arg = "-C";
        arguments[index++] = arg.data();
        arguments[index++] = cwd;
    }

    // copy remaining arguments
    for (int i = 1; i < argc; ++i)
    {
        std::string search(argv[i]);
        int p = search.find("--launcher-log");
        if (p >= 0)
            continue;
        arguments[index++] = argv[i];
    }

    // report the entire command line
    std::string complete;
    for (int i = 0; i < sz; ++i)
    {
        complete += arguments[i];
        complete += " ";
    }

    if (isLogging)
    {
        log << "complete: " << command << " ";
        log << complete << std::endl;
    }

    // run it
    pid_t pid = fork();

    if (pid == 0)
    {
        // Child process
        execv(command.c_str(), arguments);
        if (isLogging)
            log << "execl failed" << std::endl;
        return 1;
    }
    else if (pid > 0)
    {
        // Parent process
        int status;
        pid_t child_pid = waitpid(pid, &status, 0);

        if (child_pid == -1)
        {
            if (isLogging)
                log << "waitpid error" << std::endl;
            return 1;
        }

        if (isLogging)
        {
            if (WIFEXITED(status))
            {
                log << "Child process exited with status: " << WEXITSTATUS(status) << std::endl;
            }
            else if (WIFSIGNALED(status))
            {
                log << "Child process terminated by signal: " << WTERMSIG(status) << std::endl;
            }
        }
    }
    else
    {
        // Fork error
        if (isLogging)
        {
            log << "fork error" << std::endl;
            log.close();
        }
        return 1;
    }

    if (isLogging)
        log.close();

    return 0;
}
