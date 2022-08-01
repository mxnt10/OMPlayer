<html lang="">
    <body>
        <h1 align="center">
            OMPlayer - Open Multimedia Player
        </h1>
        <h3 align="center">
            Multimedia player developed in C++ using Qt5 as an interface.
            To play multimedia files, the program uses the QtAV framework,
            which uses ffmpeg for handling multimedia files.
        </h3>
        <h3 align="center">
            <a><img src="https://raw.githubusercontent.com/mxnt10/OMPlayer/master/preview/preview.png" alt=""></a>
        </h3><br/>
    </body>
</html>

## Required packages

- Qt5 >= 5.15.3
- QtAV >= 1.13.0
- libmediainfo >= 22.03
- libzen >= 0.4.39

## Shotcuts

- Ctrl+A: Open files.
- Ctrl+O: Open files.
- Ctrl+H: Shuffle mode.
- Ctrl+T: Repeat mode.
- Ctrl+Alt+T: Repeat current media (repeat one).
- Alt+Enter: Show/Exit fullscreen.
- Alt+S: Show Settings.
- Esc: Exit fullscreen.
- Multimedia Keys: Play/Pause, Next, Previous.

## Installation instructions

To install "OMPlayer", use the following commands:
```
$ git clone https://github.com/mxnt10/OMPlayer.git
$ cd OMPlayer

$ sudo su
# install_root=/usr ./install.sh
```
The script already does the compilation.

## Limitations

- The framework does not have the next and previous functions,
so it was necessary to create methods in the code to guarantee these functionalities.
- It is only possible to obtain the information of the multimedia file during its playback,
so adjustments in the code were necessary to obtain this information through other methods.

## GNU General Public License

This repository has scripts that were created to be free software.<br/>
Therefore, they can be distributed and/or modified within the terms of the *GNU General Public License*.

>[General Public License](https://pt.wikipedia.org/wiki/GNU_General_Public_License)
>
>Free Software Foundation (FSF) Inc. 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

## Comments

In case of bugs, problems of execution or construction of packages, constructive criticism, among others,
please submit a message to one of the contacts below.

## Contact

Maintainer: Mauricio Ferrari<br/>
E-Mail: *m10ferrari1200@gmail.com*<br/>
Telegram: *@maurixnovatrento*<br/>
