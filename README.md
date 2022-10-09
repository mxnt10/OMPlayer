<div align="center">
  <h1>OMPlayer - Open Multimedia Player</h1>
    <a href="/LICENSE">
        <img alt="GitHub" src="https://img.shields.io/github/license/mxnt10/OMPlayer?color=blue&label=License&style=flat-square">
    </a>
    <a href="https://github.com/mxnt10/OMPlayer/releases">
        <img src="https://img.shields.io/github/v/release/mxnt10/OMPlayer?color=blue&label=Release%20Version&style=flat-square" alt=""/>
        <img src="https://img.shields.io/github/repo-size/mxnt10/OMPlayer?color=blue&label=Repo%20Size&style=flat-square" alt=""/>
        <img src="https://img.shields.io/github/last-commit/mxnt10/OMPlayer?color=blue&label=Last%20Commit&style=flat-square" alt=""/>
        <img src="https://img.shields.io/github/release-date/mxnt10/OMPlayer?color=blue&label=Release%20Date&style=flat-square" alt=""/>
    </a>
    <a href="https://github.com/mxnt10/OMPlayer/stargazers">
        <img alt="GitHub Repo stars" src="https://img.shields.io/github/stars/mxnt10/OMPlayer?color=blue&label=GitHub%20Stars&style=flat-square">
    </a><br/><br/>
    <a><img src="https://raw.githubusercontent.com/mxnt10/OMPlayer/master/preview/preview.png" alt=""></a>
    <br/><br/>
    Multimedia player developed in C++ using Qt5 as an interface.
    To play multimedia files, the program uses the QtAV framework,
    which uses ffmpeg for handling multimedia files.
</div>

<h1>Required Packages</h1>

- Qt5 >= 5.15.3
- QtAV >= 1.13.0

<h1>Shotcuts</h1>

<h3>
🎬 <sub>Ctrl+A or Ctrl+O: Open files.</sub><br/>
🔀 <sub>Ctrl+H: Shuffle mode.</sub><br/>
🔁 <sub>Ctrl+T: Repeat mode.</sub><br/>
🔂 <sub>Ctrl+Alt+T: Repeat current media (repeat one).</sub><br/>
🖥️ <sub>Alt+Enter: Show or exit fullscreen.</sub><br/>
🛠️ <sub>Alt+S: Show Settings.</sub><br/>
🖥️ <sub>Esc: Exit fullscreen.</sub><br/>
<sub>Multimedia Keys: Play/Pause</sub> ⏯️️<sub>, Previous</sub> ⏮️<sub>, Next</sub> ⏭<sub>.</sub>
</h3>

<h1>Installation Instructions</h1>

To install OMPlayer, use the following commands:
```
$ git clone https://github.com/mxnt10/OMPlayer.git
$ cd OMPlayer

$ sudo su
# install_root=/usr ./install.sh
```
The script already does the compilation.

<h1>Limitations</h1>

- The framework does not have the next and previous functions,
so it was necessary to create methods in the code to guarantee these functionalities.

- It is only possible to obtain the information of the multimedia file during its playback,
so adjustments in the code were necessary to obtain this information through other methods.

- For multimedia controls, floating widget was used, not widget overlay. Depending on the decoding option,
the overlay prevented the video from being viewed.

<h1>GNU General Public License</h1>

This repository has scripts that were created to be free software.<br/>
Therefore, they can be distributed and/or modified within the terms of the <i>`GNU General Public License`</i>.

>
>[General Public License](https://pt.wikipedia.org/wiki/GNU_General_Public_License)
>
> Free Software Foundation (FSF) Inc. 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

<h1>Comments</h1>

In case of bugs, problems of execution or construction of packages, constructive criticism, among others,
please submit a message to one of the contacts below.

<h1>Contact</h1>

Maintainer: Mauricio Ferrari - <a href=""><i>m10ferrari1200@gmail.com</i></a><br/>
Telegram: <a href=""><i>@maurixnovatrento</i></a><br/>
