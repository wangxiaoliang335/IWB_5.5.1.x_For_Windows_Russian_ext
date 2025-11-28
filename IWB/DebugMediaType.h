#pragma once
#define BEGIN_MEDIA_TYPE_DEBUG(guid, typeguid, typeinfo)\
if(guid == typeguid)\
{\
	AtlTrace(_T("%s\r\n"),typeinfo);\
}


#define MEDIA_TYPE_DEBUG(guid, typeguid, typeinfo)\
	else if(guid == typeguid)\
{\
	AtlTrace(_T("%s\r\n"),typeinfo);\
}


#define END_MEDIA_TYPE_DEBUG(info)\
else\
{\
	AtlTrace(_T("%s\r\n"), info);\
}




inline void DebugMediaType(const AM_MEDIA_TYPE* pMediaType)
{

	if(pMediaType == NULL) return;
	GUID guidMedia = pMediaType->subtype;


	AtlTrace(_T("[Media Type]:"));
	BEGIN_MEDIA_TYPE_DEBUG( pMediaType->majortype, MEDIATYPE_AnalogAudio   , _T("Analog  audio."))
	MEDIA_TYPE_DEBUG( pMediaType->majortype, MEDIATYPE_AnalogVideo   , _T("Analog  video."))
	MEDIA_TYPE_DEBUG( pMediaType->majortype, MEDIATYPE_Audio         , _T("Audio."))
	MEDIA_TYPE_DEBUG( pMediaType->majortype, MEDIATYPE_AUXLine21Data , _T("Line  21  data.  Used  by  closed  captions."))
	MEDIA_TYPE_DEBUG( pMediaType->majortype, MEDIATYPE_File          , _T("File.  (Obsolete)"))
	MEDIA_TYPE_DEBUG( pMediaType->majortype, MEDIATYPE_Interleaved   , _T("Interleaved  audio  and  video.  Used  for  Digital  Video  (DV)."))
	MEDIA_TYPE_DEBUG( pMediaType->majortype, MEDIATYPE_LMRT          , _T("Obsolete.  Do  not  use."))
	MEDIA_TYPE_DEBUG( pMediaType->majortype, MEDIATYPE_Midi          , _T("MIDI  format."))
	MEDIA_TYPE_DEBUG( pMediaType->majortype, MEDIATYPE_MPEG2_PES     , _T("MPEG-2  PES  packets."))
	MEDIA_TYPE_DEBUG( pMediaType->majortype, MEDIATYPE_MPEG2_SECTIONS, _T("MPEG-2  section  data"))
	MEDIA_TYPE_DEBUG( pMediaType->majortype, MEDIATYPE_ScriptCommand , _T("Data  is  a  script  command,  used  by  closed  captions."))
	MEDIA_TYPE_DEBUG( pMediaType->majortype, MEDIATYPE_Stream        , _T("Byte  stream  with  no  time  stamps."))
	MEDIA_TYPE_DEBUG( pMediaType->majortype, MEDIATYPE_Text          , _T("Text."))
	MEDIA_TYPE_DEBUG( pMediaType->majortype, MEDIATYPE_Timecode      , _T("Timecode  data.  Note:  DirectShow  does  not  provide  any  filters  that  support  this  media  type."))
	MEDIA_TYPE_DEBUG( pMediaType->majortype, MEDIATYPE_URL_STREAM    , _T("Obsolete.  Do  not  use."))
	MEDIA_TYPE_DEBUG( pMediaType->majortype, MEDIATYPE_Video         , _T("Video."))
	END_MEDIA_TYPE_DEBUG(_T("Unknown media type"))


     AtlTrace(_T("[Submedia Type]:"));
	BEGIN_MEDIA_TYPE_DEBUG( pMediaType->subtype, MEDIASUBTYPE_AYUV, _T("Format: AYUV    , Sampling: 4:4:4       , Packed Or Planar: Packed, Bits Per Channel: 8"))
	MEDIA_TYPE_DEBUG( pMediaType->subtype, MEDIASUBTYPE_YUY2,   _T("Format: YUY2          , Sampling: 4:2:2       , Packed Or Planar: Packed, Bits Per Channel: 8"))
	MEDIA_TYPE_DEBUG( pMediaType->subtype, MEDIASUBTYPE_UYVY,   _T("Format: UYVY          , Sampling: 4:2:2       , Packed Or Planar: Packed, Bits Per Channel: 8"))
	MEDIA_TYPE_DEBUG( pMediaType->subtype, MEDIASUBTYPE_IMC1,   _T("Format: IMC1          , Sampling: 4:2:0       , Packed Or Planar: Planar, Bits Per Channel: 8"))
	MEDIA_TYPE_DEBUG( pMediaType->subtype, MEDIASUBTYPE_IMC3,   _T("Format: IMC2          , Sampling: 4:2:0       , Packed Or Planar: Planar, Bits Per Channel: 8"))
	MEDIA_TYPE_DEBUG( pMediaType->subtype, MEDIASUBTYPE_IMC2,   _T("Format: IMC3          , Sampling: 4:2:0       , Packed Or Planar: Planar, Bits Per Channel: 8"))
	MEDIA_TYPE_DEBUG( pMediaType->subtype, MEDIASUBTYPE_IMC4,   _T("Format: IMC4          , Sampling: 4:2:0       , Packed Or Planar: Planar, Bits Per Channel: 8"))
	MEDIA_TYPE_DEBUG( pMediaType->subtype, MEDIASUBTYPE_YV12,   _T("Format: YV12          , Sampling: 4:2:0       , Packed Or Planar: Planar, Bits Per Channel: 8"))
	MEDIA_TYPE_DEBUG( pMediaType->subtype, MEDIASUBTYPE_NV12,   _T("Format: NV12          , Sampling: 4:2:0       , Packed Or Planar: Planar, Bits Per Channel: 8"))
	MEDIA_TYPE_DEBUG( pMediaType->subtype, MEDIASUBTYPE_Y411,   _T("Format: Y411          , Sampling: 4:1:1       , Packed Or Planar: Packed, Bits Per Channel: 8"))
	MEDIA_TYPE_DEBUG( pMediaType->subtype, MEDIASUBTYPE_Y41P,   _T("Format: Y41P          , Sampling: 4:1:1       , Packed Or Planar: Packed, Bits Per Channel: 8"))
	MEDIA_TYPE_DEBUG( pMediaType->subtype, MEDIASUBTYPE_Y211,   _T("Format: Y211          , Sampling: See remarks., Packed Or Planar: Packed, Bits Per Channel: 8"))
	MEDIA_TYPE_DEBUG( pMediaType->subtype, MEDIASUBTYPE_YVYU,   _T("Format: YVYU          , Sampling: 4:2:2       , Packed Or Planar: Packed, Bits Per Channel: 8"))
	MEDIA_TYPE_DEBUG( pMediaType->subtype, MEDIASUBTYPE_YVU9,   _T("Format: YVU9          , Sampling: See remarks., Packed Or Planar: Planar, Bits Per Channel: 8"))
	MEDIA_TYPE_DEBUG( pMediaType->subtype, MEDIASUBTYPE_IF09,   _T("Format: Indeo YVU9YVU9, Sampling: See remarks., Packed Or Planar: Planar, Bits Per Channel: 8"))
	MEDIA_TYPE_DEBUG( pMediaType->subtype, MEDIASUBTYPE_ARGB32, _T("Format: Video AGB32, "))
	END_MEDIA_TYPE_DEBUG(_T("Unkown submedia type"))
}