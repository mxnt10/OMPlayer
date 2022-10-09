<div align="center">
  <h1>OMPlayer - Open Multimedia Player</h1>
  <h3>
      <a><img src="https://raw.githubusercontent.com/mxnt10/OMPlayer/master/preview/preview.png" alt=""></a>
      <br/><br/>
      Multimedia player developed in C++ using Qt5 as an interface.
      To play multimedia files, the program uses the QtAV framework,
      which uses ffmpeg for handling multimedia files.
  </h3>
</div>

<h2>Required Packages</h2>

- <b>Qt5</b> >= 5.15.3
- <b>QtAV</b> >= 1.13.0

<h2>Shotcuts</h2>

- 🎬 Ctrl+A or Ctrl+O: Open files.
- 🔀 Ctrl+H: Shuffle mode.
- 🔁 Ctrl+T: Repeat mode.
- 🔂 Ctrl+Alt+T: Repeat current media (repeat one).
- 🖥️ Alt+Enter: Show or exit fullscreen.
- 🛠️ Alt+S: Show Settings.
- 🖥️ Esc: Exit fullscreen.
- Multimedia Keys: Play/Pause ⏯️️ , Previous ⏮️ , Next ⏭ .

<h2>Installation Instructions</h2>

To install <b>OMPlayer</b>, use the following commands:
```
$ git clone https://github.com/mxnt10/OMPlayer.git
$ cd OMPlayer

$ sudo su
# install_root=/usr ./install.sh
```
The script already does the compilation.

<h2>Limitations</h2>

- The framework does not have the next and previous functions,
so it was necessary to create methods in the code to guarantee these functionalities.

- It is only possible to obtain the information of the multimedia file during its playback,
so adjustments in the code were necessary to obtain this information through other methods.

- For multimedia controls, floating widget was used, not widget overlay. Depending on the decoding option,
the overlay prevented the video from being viewed.

<h2>GNU General Public License</h2>

This repository has scripts that were created to be free software.<br/>
Therefore, they can be distributed and/or modified within the terms of the <b><i>GNU General Public License</i></b>.

>
>[General Public License](https://pt.wikipedia.org/wiki/GNU_General_Public_License)
>
> Free Software Foundation (FSF) Inc. 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

<h2>Comments</h2>

In case of bugs, problems of execution or construction of packages, constructive criticism, among others,
please submit a message to one of the contacts below.

<h2>Contact</h2>

Maintainer: <b>Mauricio Ferrari</b> - <a href=""><i>m10ferrari1200@gmail.com</i></a><br/>
Telegram: <a href=""><i>@maurixnovatrento</i></a><br/>
