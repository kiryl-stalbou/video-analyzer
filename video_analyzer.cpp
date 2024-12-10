#include <mfidl.h>
#include <mfreadwrite.h>
#include <mfapi.h>
#include <shlwapi.h>

#include "video_analyzer.h"
#include "utils.h"



HRESULT VideoAnalyzer::readInfo(const wstring& filePath, VideoInfo& videoInfo) {
    HRESULT hr = S_OK;

    IMFSourceReader* imfSourceReader = nullptr;
    IMFMediaType* imfMediaType = nullptr;

    WIN32_FILE_ATTRIBUTE_DATA fileAttributeData;

    GUID codecGuid;

    UINT32 width = 0;
    UINT32 height = 0;
    UINT32 numerator = 0;
    UINT32 denominator = 0;
    UINT32 bitRate = 0;

    PROPVARIANT varDuration;
    PropVariantInit(&varDuration);

    hr = MFCreateSourceReaderFromURL(filePath.c_str(), nullptr, &imfSourceReader);

    if (SUCCEEDED(hr))
    {
        hr = imfSourceReader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, &imfMediaType);
    }

    if (SUCCEEDED(hr))
    {
        hr = imfSourceReader->GetPresentationAttribute(MF_SOURCE_READER_MEDIASOURCE, MF_PD_DURATION, &varDuration);      
    }

    if (SUCCEEDED(hr) && varDuration.vt == VT_UI8)
    {
        videoInfo.duration = varDuration.uhVal.QuadPart / 10000000.0;
    }

    if (SUCCEEDED(hr))
    {
        hr = MFGetAttributeSize(imfMediaType, MF_MT_FRAME_SIZE, &width, &height);
    }

    if (SUCCEEDED(hr))
    {
        videoInfo.width = width;
        videoInfo.height = height;
    }

    if (SUCCEEDED(hr))
    {
        hr = MFGetAttributeRatio(imfMediaType, MF_MT_FRAME_RATE, &numerator, &denominator);
    }
    
    if (SUCCEEDED(hr) && numerator != 0 && denominator != 0)
    {
        videoInfo.frameRate = static_cast<double>(numerator) / denominator;
        videoInfo.frameDuration = 1000.0 / videoInfo.frameRate;
        videoInfo.frameCount = static_cast<UINT32>(videoInfo.duration * videoInfo.frameRate);
    }

    if (SUCCEEDED(hr))
    {
        hr = imfMediaType->GetUINT32(MF_MT_AVG_BITRATE, &bitRate);
    }

    if (SUCCEEDED(hr)) 
    {
        videoInfo.bitRate = bitRate / 1000;
    }
    
    if (SUCCEEDED(hr))
    {
        hr = imfMediaType->GetGUID(MF_MT_SUBTYPE, &codecGuid);
    }
    
    if (SUCCEEDED(hr))
    {
        videoInfo.codecName = _codecNameBy(codecGuid);
    }

    if (GetFileAttributesEx(filePath.c_str(), GetFileExInfoStandard, &fileAttributeData))
    {
        ULARGE_INTEGER fileSize;
        fileSize.HighPart = fileAttributeData.nFileSizeHigh;
        fileSize.LowPart = fileAttributeData.nFileSizeLow;
        videoInfo.fileSize = fileSize.QuadPart;
        videoInfo.fileName = PathFindFileName(filePath.c_str());
        videoInfo.fileType = PathFindExtension(filePath.c_str());
        videoInfo.filePath = filePath;
    }

    PropVariantClear(&varDuration);
    SafeRelease(&imfMediaType);
    SafeRelease(&imfSourceReader);

    return hr;
}

wstring VideoAnalyzer::_codecNameBy(const GUID& codecGuid) {
    if (codecGuid == MFVideoFormat_RGB32) return L"RGB32";
    if (codecGuid == MFVideoFormat_ARGB32) return L"ARGB32";
    if (codecGuid == MFVideoFormat_RGB24) return L"RGB24";
    if (codecGuid == MFVideoFormat_RGB555) return L"RGB555";
    if (codecGuid == MFVideoFormat_RGB565) return L"RGB565";
    if (codecGuid == MFVideoFormat_RGB8) return L"RGB8";
    if (codecGuid == MFVideoFormat_L8) return L"L8";
    if (codecGuid == MFVideoFormat_L16) return L"L16";
    if (codecGuid == MFVideoFormat_D16) return L"D16";
    if (codecGuid == MFVideoFormat_AI44) return L"AI44";
    if (codecGuid == MFVideoFormat_AYUV) return L"AYUV";
    if (codecGuid == MFVideoFormat_YUY2) return L"YUY2";
    if (codecGuid == MFVideoFormat_YVYU) return L"YVYU";
    if (codecGuid == MFVideoFormat_YVU9) return L"YVU9";
    if (codecGuid == MFVideoFormat_UYVY) return L"UYVY";
    if (codecGuid == MFVideoFormat_NV11) return L"NV11";
    if (codecGuid == MFVideoFormat_NV12) return L"NV12";
    if (codecGuid == MFVideoFormat_NV21) return L"NV21";
    if (codecGuid == MFVideoFormat_YV12) return L"YV12";
    if (codecGuid == MFVideoFormat_I420) return L"I420";
    if (codecGuid == MFVideoFormat_IYUV) return L"IYUV";
    if (codecGuid == MFVideoFormat_Y210) return L"Y210";
    if (codecGuid == MFVideoFormat_Y216) return L"Y216";
    if (codecGuid == MFVideoFormat_Y410) return L"Y410";
    if (codecGuid == MFVideoFormat_Y416) return L"Y416";
    if (codecGuid == MFVideoFormat_Y41P) return L"Y41P";
    if (codecGuid == MFVideoFormat_Y41T) return L"Y41T";
    if (codecGuid == MFVideoFormat_Y42T) return L"Y42T";
    if (codecGuid == MFVideoFormat_P210) return L"P210";
    if (codecGuid == MFVideoFormat_P216) return L"P216";
    if (codecGuid == MFVideoFormat_P010) return L"P010";
    if (codecGuid == MFVideoFormat_P016) return L"P016";
    if (codecGuid == MFVideoFormat_v210) return L"v210";
    if (codecGuid == MFVideoFormat_v216) return L"v216";
    if (codecGuid == MFVideoFormat_v410) return L"v410";
    if (codecGuid == MFVideoFormat_MP43) return L"MP43";
    if (codecGuid == MFVideoFormat_MP4S) return L"MP4S";
    if (codecGuid == MFVideoFormat_M4S2) return L"M4S2";
    if (codecGuid == MFVideoFormat_MP4V) return L"MP4V";
    if (codecGuid == MFVideoFormat_WMV1) return L"WMV1";
    if (codecGuid == MFVideoFormat_WMV2) return L"WMV2";
    if (codecGuid == MFVideoFormat_WMV3) return L"WMV3";
    if (codecGuid == MFVideoFormat_WVC1) return L"WVC1";
    if (codecGuid == MFVideoFormat_MSS1) return L"MSS1";
    if (codecGuid == MFVideoFormat_MSS2) return L"MSS2";
    if (codecGuid == MFVideoFormat_MPG1) return L"MPG1";
    if (codecGuid == MFVideoFormat_DVSL) return L"DVSL";
    if (codecGuid == MFVideoFormat_DVSD) return L"DVSD";
    if (codecGuid == MFVideoFormat_DVHD) return L"DVHD";
    if (codecGuid == MFVideoFormat_DV25) return L"DV25";
    if (codecGuid == MFVideoFormat_DV50) return L"DV50";
    if (codecGuid == MFVideoFormat_DVH1) return L"DVH1";
    if (codecGuid == MFVideoFormat_DVC) return L"DVC";
    if (codecGuid == MFVideoFormat_H264) return L"H.264";
    if (codecGuid == MFVideoFormat_H265) return L"H.265";
    if (codecGuid == MFVideoFormat_MJPG) return L"MJPG";
    if (codecGuid == MFVideoFormat_420O) return L"420O";
    if (codecGuid == MFVideoFormat_HEVC) return L"HEVC";
    if (codecGuid == MFVideoFormat_HEVC_ES) return L"HEVC_ES";
    if (codecGuid == MFVideoFormat_VP80) return L"VP8";
    if (codecGuid == MFVideoFormat_VP90) return L"VP9";
    if (codecGuid == MFVideoFormat_ORAW) return L"ORAW";

    return L"Unknown Codec";
}