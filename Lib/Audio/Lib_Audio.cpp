#include "Lib_Audio.h"
#include <assert.h>
#include <mmsystem.h>
#include <tchar.h>
#include <dsound.h>
#include <string>

// Waveファイルオープン関数
bool openWave(TCHAR* filepath, WAVEFORMATEX& waveFormatEx, char** ppData, DWORD& dataSize) {
	if (filepath == 0)
		return false;

	HMMIO hMmio = NULL;
	MMIOINFO mmioInfo;

	// Waveファイルオープン
	memset(&mmioInfo, 0, sizeof(MMIOINFO));
	hMmio = mmioOpen(filepath, &mmioInfo, MMIO_READ);
	if (!hMmio)
		return false; // ファイルオープン失敗

	// RIFFチャンク検索
	MMRESULT mmRes;
	MMCKINFO riffChunk;
	riffChunk.fccType = mmioFOURCC('W', 'A', 'V', 'E');
	mmRes = mmioDescend(hMmio, &riffChunk, NULL, MMIO_FINDRIFF);
	if (mmRes != MMSYSERR_NOERROR) {
		mmioClose(hMmio, 0);
		return false;
	}

	// フォーマットチャンク検索
	MMCKINFO formatChunk;
	formatChunk.ckid = mmioFOURCC('f', 'm', 't', ' ');
	mmRes = mmioDescend(hMmio, &formatChunk, &riffChunk, MMIO_FINDCHUNK);
	if (mmRes != MMSYSERR_NOERROR) {
		mmioClose(hMmio, 0);
		return false;
	}
	DWORD fmsize = formatChunk.cksize;
	DWORD size = mmioRead(hMmio, (HPSTR)&waveFormatEx, fmsize);
	if (size != fmsize) {
		mmioClose(hMmio, 0);
		return false;
	}

	mmioAscend(hMmio, &formatChunk, 0);

	// データチャンク検索
	MMCKINFO dataChunk;
	dataChunk.ckid = mmioFOURCC('d', 'a', 't', 'a');
	mmRes = mmioDescend(hMmio, &dataChunk, &riffChunk, MMIO_FINDCHUNK);
	if (mmRes != MMSYSERR_NOERROR) {
		mmioClose(hMmio, 0);
		return false;
	}
	*ppData = new char[dataChunk.cksize];
	size = mmioRead(hMmio, (HPSTR)*ppData, dataChunk.cksize);
	if (size != dataChunk.cksize) {
		delete[] * ppData;
		return false;
	}
	dataSize = size;

	// ハンドルクローズ
	mmioClose(hMmio, 0);

	return true;
}

Lib_AudioBase::Lib_AudioBase()
{
}

Lib_AudioBase::~Lib_AudioBase()
{
	pDSBuffer->Release();
}

#define len (1024)

void Lib_AudioBase::CreateAudio(IDirectSound8* pDS8, const std::string fileName)
{
	//std::string text = ;
	const char *src = fileName.c_str();
	TCHAR dst[2048];
	memset(&dst,0, sizeof(dst));
	MultiByteToWideChar(CP_ACP, 0, &src[0], sizeof(const char* )* fileName.size(), ((LPWSTR)&dst[0]), len);
	
	// Waveファイルオープン
	WAVEFORMATEX wFmt;
	char* pWaveData = 0;
	DWORD waveSize = 0;
	if (!openWave(dst, wFmt, &pWaveData, waveSize))
	{
		assert(!"CreateAudioエラー");
		return;
	}

	DSBUFFERDESC DSBufferDesc;
	DSBufferDesc.dwSize = sizeof(DSBUFFERDESC);
	DSBufferDesc.dwFlags = 0;
	DSBufferDesc.dwBufferBytes = waveSize;
	DSBufferDesc.dwReserved = 0;
	DSBufferDesc.lpwfxFormat = &wFmt;
	DSBufferDesc.guid3DAlgorithm = GUID_NULL;

	IDirectSoundBuffer* ptmpBuf = 0;
	pDS8->CreateSoundBuffer(&DSBufferDesc, &ptmpBuf, NULL);
	ptmpBuf->QueryInterface(IID_IDirectSoundBuffer8, (void**)&pDSBuffer);
	ptmpBuf->Release();
	if (pDSBuffer == 0) {
		pDS8->Release();
		assert(!"CreateAudioエラー");
		return;
	}
	
	
	// セカンダリバッファにWaveデータ書き込み
	LPVOID lpvWrite = 0;
	DWORD dwLength = 0;
	if (DS_OK == pDSBuffer->Lock(0, 0, &lpvWrite, &dwLength, NULL, NULL, DSBLOCK_ENTIREBUFFER)) {
		memcpy(lpvWrite, pWaveData, dwLength);
		pDSBuffer->Unlock(lpvWrite, dwLength, NULL, 0);
	}
	delete[] pWaveData; // 元音はもういらない
}

void Lib_AudioBase::Play(bool loop)
{
	if (loop)
	{
		pDSBuffer->Play(0, 0, DSBPLAY_LOOPING);
		return;
	}
	pDSBuffer->Play(0, 0, 0);
}	

void Lib_AudioBase::Stop()
{
	pDSBuffer->Stop();

}
void Lib_AudioBase::SetVolume(const LONG &volume)
{
	pDSBuffer->SetVolume(volume);
}
void Lib_AudioBase::SetPan(const LONG& pan)
{
	pDSBuffer->SetPan(pan);
}

