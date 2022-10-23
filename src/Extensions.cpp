#include "Extensions.h"


/**********************************************************************************************************************/


/** Construtor */
ExtensionList::ExtensionList() : QStringList() {}


/**********************************************************************************************************************/


/** Usado para montar o filtro para abrir arquivos para o reprodutor */
QString ExtensionList::forFilter() {
	QString s;
	for (int i = 0; i < count(); i++) s = s + "*." + at(i) + " ";
	if (!s.isEmpty()) s = " (" + s + ")";
	return s;
}


/***/
QStringList ExtensionList::forDirFilter() {
	QStringList l;
	for (int i = 0; i < count(); i++) {
		QString s = "*." + at(i);
		l << s;
	}
	return l;
}


/***/
QString ExtensionList::forRegExp() {
	QString s;
	for (int n=0; n < count(); n++) {
		if (!s.isEmpty()) s = s + "|";
		s = s + "^" + at(n) + "$";
	}
	return s;
}


/**********************************************************************************************************************/


/** Construtor */
Extensions::Extensions() {

    _3gp << "3ga" << "3gp" << "3gpp";
    _3gp2 << "3g2" << "3gp2" << "3gpp2";
    _avi << "avf" << "avi" << "divx";
    _flash << "flv" << "f4v";
    _mkv << "mkv";
    _mpeg << "m1v" << "mpe" << "mpeg" << "mpg" << "ts" << "vob" << "vdr" << "mpv";
    _mpeg2 << "m2v" << "mp2" << "m2t" << "m2ts";
    _mpeg4 << "f4v" << "lrv" << "m4v" << "mp4" << "m4b";
    _ogg << "ogg" << "ogv" << "ogm" << "ogx";
    _quicktime << "moov" << "mov" << "qt" << "qtvr" << "hdmov";
    _realmedia << "rv" << "rvx" << "rmvb" << "rm";
    _webm << "webm" << "vp8";
    _wmv << "wmv" << "wmp" << "wtv" << "dvr-ms" << "asf" << "vfw";
    _other_video << "iso" << "bin" << "dat" << "dv" << "mqv" << "vcd" << "nsv" << "mts" << "rec" << "bik" << "smk";

    _acc << "aac" << "adts" << "ass";
    _dolby << "ac3";
    _flac << "flac";
    _mka << "mka";
    _midi << "kar" << "mid" << "midi";
    _m4a << "f4a" << "m4a";
    _mp3 << "mp3" << "mpga";
    _oga << "oga" << "ogg" << "opus" << "spx";
    _realaudio << "ra" << "ram";
    _wma << "wma" << "asf";
    _wav << "wav" << "wp" << "wvp" << "wv";
    _other_audio << "mpc" << "dts" << "ape" << "thd" << "shn";

	_subtitles << "srt" << "sub" << "ssa" << "ass" << "idx" << "txt"
               << "smi" << "rt" << "utf" << "aqt" << "vtt" << "mpsub";

	_playlist << "m3u" << "m3u8" << "pls" << "xspf";

    _video << _3gp << _3gp2 << _avi << _flash << _mkv << _mpeg << _mpeg2 << _mpeg4 << _ogg
           << _quicktime << _realmedia << _webm << _wmv << _other_video;

    _audio << _acc << _dolby << _flac << _mka << _midi << _m4a << _mp3 << _realaudio <<_oga
           << _wma << _wav << _other_audio;

	_multimedia << _video << _audio;
	_all_playable << _multimedia << _playlist;
}


/** Destrutor */
Extensions::~Extensions() = default;
