#include <mfapi.h>

#include "video_analyzer.h"
#include "video_reader.h"
#include "utils.h"



VideoReader::VideoReader(VideoInfo videoInfo) : _videoInfo(videoInfo)
{
    _configureSourceReader();
}

VideoReader::~VideoReader() 
{
    SafeRelease(&_imfSourceReader);
}

HRESULT VideoReader::readFrame(BYTE** bytes)
{
    HRESULT hr = S_OK;
    DWORD imfSampleFlags = 0;
    IMFSample* imfSample = nullptr;
    IMFMediaBuffer* imfBuffer = nullptr;

    hr = _imfSourceReader->ReadSample(MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, nullptr, &imfSampleFlags, nullptr, &imfSample);

    if (SUCCEEDED(hr) && (imfSampleFlags & MF_SOURCE_READERF_ENDOFSTREAM))
    {   
        hr = E_FAIL;
    }

    if (SUCCEEDED(hr))
    {
        hr = imfSample->ConvertToContiguousBuffer(&imfBuffer);
    }

    if (SUCCEEDED(hr))
    {
        DWORD bufferSize = 0;
        BYTE* buffer = nullptr;
        hr = imfBuffer->Lock(&buffer, nullptr, &bufferSize);

        currentFrameIndex++;
        *bytes = new BYTE[bufferSize];
        memcpy(*bytes, buffer, bufferSize);

        imfBuffer->Unlock();
    }

    SafeRelease(&imfBuffer);
    SafeRelease(&imfSample);

    return hr;
}

HRESULT VideoReader::seekFrame(UINT32 frameIndex)
{
    HRESULT hr = S_OK;
    double frameTime = frameIndex / _videoInfo.frameRate;
    LONGLONG framePosition = static_cast<LONGLONG>(frameTime * 10000000.0);

    PROPVARIANT varFramePosition;
    PropVariantInit(&varFramePosition);

    varFramePosition.vt = VT_I8;
    varFramePosition.hVal.QuadPart = framePosition;
    hr = _imfSourceReader->SetCurrentPosition(GUID_NULL, varFramePosition);

    if (SUCCEEDED(hr))
    {
        currentFrameIndex = frameIndex;
    }

    PropVariantClear(&varFramePosition);

    return hr;
}

HRESULT VideoReader::_configureSourceReader()
{
    HRESULT hr = S_OK;
    IMFAttributes* imfAttributes = NULL;

    hr = MFCreateAttributes(&imfAttributes, 1);

    if (SUCCEEDED(hr))
    {
        hr = imfAttributes->SetUINT32(MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING, TRUE);
    }

    if (SUCCEEDED(hr))
    {
        hr = MFCreateSourceReaderFromURL(_videoInfo.filePath.c_str(), imfAttributes, &_imfSourceReader);
    }

    if (SUCCEEDED(hr))
    {
        hr = _selectSourceVideoStream();
    }

    SafeRelease(&imfAttributes);

    return hr;
}

HRESULT VideoReader::_selectSourceVideoStream()
{
    HRESULT hr = S_OK;
    IMFMediaType* imfMediaType = NULL;

    hr = MFCreateMediaType(&imfMediaType);

    if (SUCCEEDED(hr))
    {
        hr = imfMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
    }

    if (SUCCEEDED(hr))
    {
        hr = imfMediaType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32);
    }

    if (SUCCEEDED(hr))
    {
        hr = _imfSourceReader->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, NULL, imfMediaType);
    }

    if (SUCCEEDED(hr))
    {
        hr = _imfSourceReader->SetStreamSelection((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, TRUE);
    }

    SafeRelease(&imfMediaType);

    return hr;
}
