//Copyright (c) 2022 Ultimaker B.V.
//CuraEngine is released under the terms of the AGPLv3 or higher.

#ifndef APPLICATION_H
#define APPLICATION_H

#include "utils/NoCopy.h"
#include <cstddef> //For size_t.
#include <vector> //To track all communication channels.

namespace cura
{
class Communication;
class Slice;

/*!
 * A singleton class that serves as the starting point for all slicing.
 *
 * The application provides a starting point for the slicing engine. It
 * maintains communication with other applications and uses that to schedule
 * slices.
 */
class Application : NoCopy
{
public:
    /*
     * \brief The communication channels currently in use.
     *
     * This will be filled with communication channels during the initialization
     * of the program. Anything that must be communicated must be communicated
     * to all of these. Slices will be processed from these in round-robin
     * style.
     */
    std::vector<Communication*> communications;

    /*
     * \brief The slice that is currently ongoing.
     *
     * If no slice has started yet, this will be a nullptr.
     */
    Slice* current_slice;

    /*!
     * Gets the instance of this application class.
     */
    static Application& getInstance();

    /*!
     * \brief Print to the stderr channel what the original call to the executable was.
     */
    void printCall() const;

    /*!
     * \brief Print to the stderr channel how to use CuraEngine.
     */
    void printHelp() const;

    /*!
     * \brief Starts the application.
     *
     * It will start by parsing the command line arguments to see what it must
     * be doing.
     *
     * This function can only be called once, because it has side-effects on
     * static fields across the application.
     * \param argc The number of arguments provided to the application.
     * \param argv The arguments provided to the application.
     */
    void run(const size_t argc, char** argv);

protected:
#ifdef ARCUS
    /*!
     * \brief Connect using libArcus to a socket.
     * \param argc The number of arguments provided to the application.
     * \param argv The arguments provided to the application.
     */
    void connect();
#endif //ARCUS

    /*!
     * \brief Print the header and license to the stderr channel.
     */
    void printLicense() const;

    /*!
     * \brief Start slicing.
     * \param argc The number of arguments provided to the application.
     * \param argv The arguments provided to the application.
     */
    void slice();

private:
    /*
     * \brief The number of arguments that the application was called with.
     */
    size_t argc;

    /*
     * \brief An array of C strings containing the arguments that the
     * application was called with.
     */
    char** argv;

    /*!
     * \brief Constructs a new Application instance.
     *
     * You cannot call this because this goes via the getInstance() function.
     */
    Application();

    /*!
     * \brief Destroys the Application instance.
     *
     * This destroys the Communication instance along with it.
     */
    ~Application();
};

} //Cura namespace.

#endif //APPLICATION_H