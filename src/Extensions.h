#ifndef OMPLAYER_EXTENSIONS_H
#define OMPLAYER_EXTENSIONS_H

#include <QStringList>

class ExtensionList : public QStringList {

public:
	ExtensionList();
	QString forFilter();
};

class Extensions {

public:
	Extensions();
	~Extensions();
    QString filters();

	ExtensionList video() { return _video; };
	ExtensionList audio() { return _audio; };
	ExtensionList playlist() { return _playlist; };
	ExtensionList subtitles() { return _subtitles; };
	ExtensionList multimedia() { return _multimedia; };
	ExtensionList allPlayable() { return _all_playable; };

protected:
    ExtensionList _3gp, _3gp2, _avi, _flash, _mkv, _mpeg, _mpeg2, _mpeg4, _ogg, _quicktime, _realmedia, _webm, _wmv;
    ExtensionList _acc, _dolby, _flac, _mka, _midi, _m4a, _mp3, _oga, _realaudio, _wma, _wav;
	ExtensionList _video, _other_video, _audio, _other_audio, _playlist, _subtitles;
	ExtensionList _multimedia, _all_playable;
};

#endif //OMPLAYER_EXTENSIONS_H
