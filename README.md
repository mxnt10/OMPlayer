<div align="center">
  <h1>𝗢𝗠𝗣𝗹𝗮𝘆𝗲𝗿 - 𝗢𝗽𝗲𝗻 𝗠𝗲𝗱𝗶𝗮 𝗣𝗹𝗮𝘆𝗲𝗿</h1>
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
    Multimediɑ plɑγeɾ developed in C++ using Qt5 ɑs ɑn inteɾfɑce.
    To plɑγ multimediɑ files, the pɾogɾɑm uses the QtAV fɾɑmewoɾk,
    which uses ffmpeg foɾ hɑndling multimediɑ files.
</div>

<h3>𝖱𝖾𝗊𝗎𝗂𝗋𝖾𝖽 𝖯𝖺𝖼𝗄𝖺𝗀𝖾𝗌</h3>

- 𝐐𝐭𝟓 >= 5.15.3
- 𝐐𝐭𝐀𝐕 >= 1.13.0

<h3>Shotcuts
<ul>
    <li>📂 <sub>Ctrl+A or Ctrl+O: Open files.</sub></li>
    <li>🔀 <sub>Ctrl+H: Shuffle mode.</sub></li>
    <li>🔁 <sub>Ctrl+T: Repeat mode.</sub></li>
    <li>🔂 <sub>Ctrl+Alt+T: Repeat current media (repeat one).</sub></li>
    <li>🖥️ <sub>Alt+Enter: Show or exit fullscreen.</sub></li>
    <li>🛠️ <sub>Alt+S: Show Settings.</sub></li>
    <li>🖥️ <sub>Esc: Exit fullscreen.</sub></li>
    <li>🎵 <sub>Multimedia Keys: Play/Pause</sub> ⏯️️<sub>, Previous</sub> ⏮️<sub>, Next</sub> ⏭<sub>.</sub></li>
</ul><br/>

Instɑllɑtion Instɾuctions</h3>

To install OMPlayer, use the following commands:
```
$ git clone https://github.com/mxnt10/OMPlayer.git
$ cd OMPlayer

$ sudo su
# install_root=/usr ./install.sh
```
The script already does the compilation.

<h3>Limitɑtions</h3>

- The framework does not have the next and previous functions,
so it was necessary to create methods in the code to guarantee these functionalities.

- It is only possible to obtain the information of the multimedia file during its playback,
so adjustments in the code were necessary to obtain this information through other methods.

- For multimedia controls, floating widget was used, not widget overlay. Depending on the decoding option,
the overlay prevented the video from being viewed.

<h3>GNU General Public License</h3>

This repository has scripts that were created to be free software.<br/>
Therefore, they can be distributed and/or modified within the terms of the <i>`GNU General Public License`</i>.

>
>[General Public License](https://pt.wikipedia.org/wiki/GNU_General_Public_License)
>
> Free Software Foundation (FSF) Inc. 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

<h3>Comments</h3>

In case of bugs, problems of execution or construction of packages, constructive criticism, among others,
please submit a message to one of the contacts below.

<h3>Contɑct</h3>

Maintainer: Mauricio Ferrari - <a href=""><i>m10ferrari1200@gmail.com</i></a><br/>
Telegram: <a href=""><i>@maurixnovatrento</i></a><br/>
