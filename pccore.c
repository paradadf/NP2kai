/**
 * @file	pccore.c
 * @brief	emluration core
 *
 * @author	$Author: yui $
 * @date	$Date: 2011/02/23 10:11:44 $
 */

#include	"compiler.h"
#include	"strres.h"
#include	"dosio.h"
#include	"soundmng.h"
#include	"sysmng.h"
#include	"timemng.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"gdc_sub.h"
#include	"cbuscore.h"
#include	"pc9861k.h"
#include	"mpu98ii.h"
#include	"amd98.h"
#include "bios/bios.h"
#include "bios/biosmem.h"
#include	"vram.h"
#include	"scrndraw.h"
#include	"dispsync.h"
#include	"palettes.h"
#include	"maketext.h"
#include	"maketgrp.h"
#include	"makegrph.h"
#include	"makegrex.h"
#include	"sound.h"
#include	"fmboard.h"
#include	"beep.h"
#include	"s98.h"
#include	"tms3631.h"
#include	"fdd/diskdrv.h"
#include	"diskimage/fddfile.h"
#include	"fdd/fdd_mtr.h"
#include	"wab/wab_rly.h"
#include	"fdd/sxsi.h"
#include	"font/font.h"
#include	"bmsio.h"
#if defined(SUPPORT_HOSTDRV)
#include	"hostdrv.h"
#endif
#include	"np2ver.h"
#include	"calendar.h"
#include	"timing.h"
#include	"keystat.h"
#include	"debugsub.h"
#if defined(SUPPORT_WAB)
#include	"wab.h"
#endif
#if defined(SUPPORT_CL_GD5430)
#include	"cirrus_vga_extern.h"
#endif
#if defined(SUPPORT_HRTIMER)
#include	"upd4990.h"
#endif	/* SUPPORT_HRTIMER */
#if defined(SUPPORT_IDEIO)
#include	"ideio.h"
#endif
#if defined(SUPPORT_GPIB)
#include	"cbus/gpibio.h"
#endif
#if defined(CPUCORE_IA32)
#include	"ia32/cpu.h"
#include	"ia32/instructions/fpu/fp.h"
#else
#define	CPU_VENDOR		"GenuineIntel"
#define	CPU_FAMILY	2
#define	CPU_MODEL	1
#define	CPU_STEPPING	1
#define	CPU_FEATURES		(0)
#define	CPU_FEATURES_EX		(0)
#define	CPU_BRAND_STRING	"Intel(R) 80286 Processor "
#define	CPU_FEATURES_ECX	(0)
#define	CPU_BRAND_ID_AUTO	(0xffffffff)
#endif
#include <time.h>


const OEMCHAR np2version[] = OEMTEXT(NP2VER_CORE " " NP2VER_GIT);

#if defined(_WIN32_WCE)
#define	PCBASEMULTIPLE	2
#else
#define	PCBASEMULTIPLE	20
#endif


	NP2CFG	np2cfg = {
				0, 1, 0, 32, 0, 0, 0x40,
				0, 0, 0, 0,
				{0x3e, 0xe3, 0x7b}, 0,
				0, 0, {1, 1, 6, 1, 8, 1},
				128, 0x00, 1, 
#if defined(SUPPORT_ASYNC_CPU)
				0,
#endif

				OEMTEXT("VX"), PCBASECLOCK25, PCBASEMULTIPLE, 1,
				{0x48, 0x05, 0x04, 0x08, 0x01, 0x00, 0x00, 0x6e},
				1, 13, 2, 1, 0x000000, 0xffffff,
				44100, 150, 4, 0,
				{0, 0, 0}, 0xd1, 0x7f, 0xd1, 0, 0, 1, 
				
				0x0188, 0x80, 3, 12, 12, 0xff, 0, // 118

				0x70, 1, 3, // Mate-X PCM

#if defined(SUPPORT_SOUND_SB16)
				0xd2, 3, 5,
#endif	/* SUPPORT_SOUND_SB16 */

				3, {0x0c, 0x0c, 0x08, 0x06, 0x03, 0x0c}, 100, 64, 64, 64, 64, 64,
				1, 0x82, 0,
				0, {0x17, 0x04, 0x17}, {0x0c, 0x0c, 0x02, 0x10, 0x3f, 0x3f},
#if defined(SUPPORT_FMGEN)
				1,
#endif	/* SUPPORT_FMGEN */
				3, 0, 80, 0, 0, 1,

				0, {OEMTEXT(""), OEMTEXT(""), OEMTEXT(""), OEMTEXT("")},
#if defined(SUPPORT_IDEIO)
				{OEMTEXT(""), OEMTEXT(""), OEMTEXT(""), OEMTEXT("")}, 
				{SXSIDEV_HDD, SXSIDEV_HDD, SXSIDEV_CDROM, SXSIDEV_HDD}, 
				{OEMTEXT(""), OEMTEXT(""), OEMTEXT(""), OEMTEXT("")},
				1, 1, 0, 0, 0, 0, 0, 0, 0,
#else
				{OEMTEXT(""), OEMTEXT("")},
#endif
#if defined(SUPPORT_SCSI)
				{OEMTEXT(""), OEMTEXT(""), OEMTEXT(""), OEMTEXT("")},
#endif
				OEMTEXT(""), OEMTEXT(""), OEMTEXT(""),
#if defined(SUPPORT_NET)
				OEMTEXT(""), 0,
#endif
#if defined(SUPPORT_LGY98)
				0, 0x10D0, 5, {0x00, 0x40, 0x26, 0x12, 0x34, 0x56},
#endif
#if defined(SUPPORT_WAB)
#if defined(__LIBRETRO__)
				1,
#else
				0,
#endif
#endif
#if defined(SUPPORT_CL_GD5430)
				0, 0x5B, 0, CIRRUS_MELCOWAB_OFS_DEFAULT, 0,
#endif
#if defined(SUPPORT_GPIB)
				0, 12, 1, 0, 0, 
#endif
#if defined(SUPPORT_PCI)
				0, 0, 0,
#endif
#if defined(SUPPORT_STATSAVE)
				0,			/* statsave */
#endif
				0, 0,
				0, 0xff00, 
				0, 0, 0,
				1,
				CPU_VENDOR, CPU_FAMILY, CPU_MODEL, CPU_STEPPING, CPU_FEATURES, CPU_FEATURES_EX, CPU_BRAND_STRING, OEMTEXT(""), OEMTEXT(""), CPU_BRAND_ID_AUTO, CPU_FEATURES_ECX,
				FPU_TYPE_SOFTFLOAT,
#if defined(SUPPORT_FAST_MEMORYCHECK)
				1,
#endif
				0, 0,
				1,
	};

	PCCORE	pccore = {	PCBASECLOCK25, PCBASEMULTIPLE,
						0, PCMODEL_VX, 0, 0, {0x3e, 0x73, 0x7b}, 0,
						SOUNDID_NONE, 0,
						PCBASECLOCK25 * PCBASEMULTIPLE};
	PCSTAT	pcstat = {3, TRUE, FALSE, FALSE};

//	UINT8	screenupdate = 3;
//	int		screendispflag = 1;
	UINT8	soundrenewal = 0;
//	BOOL	drawframe;
	UINT	drawcount = 0;
//	BOOL	hardwarereset = FALSE;
#if defined(SUPPORT_FMGEN)
	UINT8	enable_fmgen = 0;
#endif	/* SUPPORT_FMGEN */

#ifdef SUPPORT_ASYNC_CPU
#if !defined(__LIBRETRO__) && !defined(NP2_SDL2) && !defined(NP2_X11)
LARGE_INTEGER asynccpu_lastclock = {0};
LARGE_INTEGER asynccpu_clockpersec = {0};
LARGE_INTEGER asynccpu_clockcount = {0};
#else
UINT64 asynccpu_lastclock = 0;
UINT64 asynccpu_clockpersec = 0;
UINT64 asynccpu_clockcount = 0;
#endif
#endif

// ---------------------------------------------------------------------------

void getbiospath(OEMCHAR *path, const OEMCHAR *fname, int maxlen) {

const OEMCHAR	*p;

	p = np2cfg.biospath;
	if (p[0]) {
		file_cpyname(path, p, maxlen);
		file_setseparator(path, maxlen);
		file_catname(path, fname, maxlen);
	}
	else {
		file_cpyname(path, file_getcd(fname), maxlen);
	}
}


// ----

static void pccore_set(const NP2CFG *pConfig)
{
	UINT8	model;
	UINT32	multiple;
	UINT16	extsize;

	ZeroMemory(&pccore, sizeof(pccore));
	model = PCMODEL_VX;
	if (!milstr_cmp(pConfig->model, str_VM)) {
		model = PCMODEL_VM;
	}
	else if (!milstr_cmp(pConfig->model, str_EPSON)) {
		model = PCMODEL_EPSON | PCMODEL_VM;
	}
	pccore.model = model;

	if (np2cfg.baseclock >= ((PCBASECLOCK25 + PCBASECLOCK20) / 2))
	{
		pccore.baseclock = PCBASECLOCK25;			// 2.5MHz
		pccore.cpumode = 0;
	}
	else
	{
		pccore.baseclock = PCBASECLOCK20;			// 2.0MHz
		pccore.cpumode = CPUMODE_8MHZ;
	}
	multiple = pConfig->multiple;
	if (multiple == 0)
	{
		multiple = 1;
	}
	else if (multiple > 2048)
	{
		multiple = 2048;
	}
	pccore.multiple = multiple;
	pccore.realclock = pccore.baseclock * multiple;

	// HDDの接続 (I/Oの使用状態が変わるので..
	if (pConfig->dipsw[1] & 0x20)
	{
		pccore.hddif |= PCHDD_IDE;
#if defined(SUPPORT_IDEIO)
		sxsi_setdevtype(0x00, np2cfg.idetype[0]==SXSIDEV_CDROM ? SXSIDEV_CDROM : SXSIDEV_NC);
		sxsi_setdevtype(0x01, np2cfg.idetype[1]==SXSIDEV_CDROM ? SXSIDEV_CDROM : SXSIDEV_NC);
		sxsi_setdevtype(0x02, np2cfg.idetype[2]==SXSIDEV_CDROM ? SXSIDEV_CDROM : SXSIDEV_NC);
		sxsi_setdevtype(0x03, np2cfg.idetype[3]==SXSIDEV_CDROM ? SXSIDEV_CDROM : SXSIDEV_NC);
#endif
	}
	else
	{
		sxsi_setdevtype(0x02, SXSIDEV_NC);
		sxsi_setdevtype(0x03, SXSIDEV_NC);
	}

	// 拡張メモリ
	extsize = 0;
	if (!(pConfig->dipsw[2] & 0x80))
	{
		extsize = np2cfg.EXTMEM;
#if defined(CPUCORE_IA32)
		extsize = np2min(extsize, MEMORY_MAXSIZE);
#else
		extsize = np2min(extsize, 13);
#endif
	}
	pccore.extmem = extsize;
	CopyMemory(pccore.dipsw, pConfig->dipsw, 3);

	// サウンドボードの接続
	pccore.sound = (SOUNDID)pConfig->SOUND_SW;

	// その他CBUSの接続
	pccore.device = 0;
	if (pConfig->pc9861enable)
	{
		pccore.device |= PCCBUS_PC9861K;
	}
	if (pConfig->mpuenable)
	{
		pccore.device |= PCCBUS_MPU98;
	}
}


// --------------------------------------------------------------------------

#if !defined(DISABLE_SOUND)
static void sound_init(void)
{
	UINT rate;

	rate = np2cfg.samplingrate;
	if (sound_create(rate, np2cfg.delayms) != SUCCESS)
	{
		rate = 0;
	}
	fddmtrsnd_initialize(rate);
	wabrlysnd_initialize(rate);
	beep_initialize(rate);
	beep_setvol(np2cfg.BEEP_VOL);
	tms3631_initialize(rate);
	tms3631_setvol(np2cfg.vol14);
	opngen_initialize(rate);
	opngen_setvol(np2cfg.vol_fm);
	psggen_initialize(rate);
	psggen_setvol(np2cfg.vol_ssg);
	rhythm_initialize(rate);
	rhythm_setvol(np2cfg.vol_rhythm);
	adpcm_initialize(rate);
	adpcm_setvol(np2cfg.vol_adpcm);
	pcm86gen_initialize(rate);
	pcm86gen_setvol(np2cfg.vol_pcm);
	cs4231_initialize(rate);
	amd98_initialize(rate);
	oplgen_initialize(rate);
	oplgen_setvol(np2cfg.vol_fm);
}

static void sound_term(void) {

	soundmng_stop();
	amd98_deinitialize();
	rhythm_deinitialize();
	beep_deinitialize();
	wabrlysnd_deinitialize();
	fddmtrsnd_deinitialize();
	sound_destroy();
}
#endif

void pccore_init(void) {

	CPU_INITIALIZE();
	
	pic_initialize();

	pal_initlcdtable();
	pal_makelcdpal();
	pal_makeskiptable();
	dispsync_initialize();
	sxsi_initialize();

	font_initialize();
	font_load(np2cfg.fontfile, TRUE);
	maketext_initialize();
	makegrph_initialize();
	gdcsub_initialize();
	fddfile_initialize();

#if !defined(DISABLE_SOUND)
#if defined(SUPPORT_FMGEN)
	enable_fmgen = 0;
	if(np2cfg.usefmgen == 1)
		enable_fmgen = 1;
#ifdef WIIU
		enable_fmgen = 0;
#endif
#endif	/* SUPPORT_FMGEN */

	fmboard_construct();
	sound_init();
#endif

	rs232c_construct();
	mpu98ii_construct();
	pc9861k_initialize();

	iocore_create();

#if defined(SUPPORT_IDEIO)
	ideio_initialize();
#endif

#if defined(SUPPORT_HOSTDRV)
	hostdrv_initialize();
#endif

#if defined(SUPPORT_HRTIMER)
	upd4990_hrtimer_start();
#endif	/* SUPPORT_HRTIMER */

#if defined(SUPPORT_GPIB)
	gpibio_initialize();
#endif
}

void pccore_term(void) {
	
#if defined(SUPPORT_GPIB)
	gpibio_shutdown();
#endif

#if defined(SUPPORT_HRTIMER)
	upd4990_hrtimer_stop();
#endif	/* SUPPORT_HRTIMER */

#if defined(SUPPORT_HOSTDRV)
	hostdrv_deinitialize();
#endif

#if !defined(DISABLE_SOUND)
	sound_term();
	fmboard_destruct();
#endif

	fdd_eject(0);
	fdd_eject(1);
	fdd_eject(2);
	fdd_eject(3);
	
#if defined(SUPPORT_IDEIO)
	ideio_deinitialize();
#endif

	iocore_destroy();

	pc9861k_deinitialize();
	mpu98ii_destruct();
	rs232c_destruct();

	sxsi_alltrash();
	
	pic_deinitialize();

	CPU_SETEXTSIZE(0);	// メモリ解放
	CPU_DEINITIALIZE();
}


void pccore_cfgupdate(void) {

	BOOL	renewal;
	int		i;

	renewal = FALSE;
	for (i=0; i<8; i++)
	{
		if (np2cfg.memsw[i] != mem[MEMX_MSW + i*4])
		{
			np2cfg.memsw[i] = mem[MEMX_MSW + i*4];
			renewal = TRUE;
		}
	}
	for (i=0; i<3; i++)
	{
		if (np2cfg.dipsw[i] != pccore.dipsw[i])
		{
			np2cfg.dipsw[i] = pccore.dipsw[i];
			renewal = TRUE;
		}
	}
	if (renewal) {
		sysmng_update(SYS_UPDATECFG);
	}
}

/**
 * Reset the virtual machine
 */
void pccore_reset(void) {

	int		i;
	BOOL	epson;

	soundmng_stop();
#if !defined(DISABLE_SOUND)
#if defined(SUPPORT_FMGEN)
	enable_fmgen = 0;
	if(np2cfg.usefmgen == 1)
		enable_fmgen = 1;
#ifdef WIIU
		enable_fmgen = 0;
#endif
#endif	/* SUPPORT_FMGEN */

	if (soundrenewal) {
		soundrenewal = 0;
		sound_term();
		sound_init();
	}
#endif
	ZeroMemory(mem, 0x110000);
	FillMemory(mem + 0xC0000, 0xE8000 - 0xC0000, 0xff); // なぞ
	ZeroMemory(mem + VRAM1_B, 0x18000);
	ZeroMemory(mem + VRAM1_E, 0x08000);
	ZeroMemory(mem + FONT_ADRS, 0x08000);

	//メモリスイッチ
	for (i=0; i<8; i++)
	{
		mem[0xa3fe2 + i*4] = np2cfg.memsw[i];
	}
	
#if defined(CPUCORE_IA32)
	if(np2cfg.cpu_family == CPU_I486SX_FAMILY && np2cfg.cpu_model == CPU_I486SX_MODEL){
		strcpy(np2cfg.cpu_vendor, CPU_VENDOR_INTEL);
		strcpy(np2cfg.cpu_brandstring, CPU_BRAND_STRING_I486SX);
	}else if(np2cfg.cpu_family == CPU_I486DX_FAMILY && np2cfg.cpu_model == CPU_I486DX_MODEL){
		strcpy(np2cfg.cpu_vendor, CPU_VENDOR_INTEL);
		strcpy(np2cfg.cpu_brandstring, CPU_BRAND_STRING_I486DX);
	}else if(np2cfg.cpu_family == CPU_PENTIUM_FAMILY && np2cfg.cpu_model == CPU_PENTIUM_MODEL && !(np2cfg.cpu_feature_ex & CPU_FEATURE_EX_3DNOW)){
		strcpy(np2cfg.cpu_vendor, CPU_VENDOR_INTEL);
		strcpy(np2cfg.cpu_brandstring, CPU_BRAND_STRING_PENTIUM);
	}else if(np2cfg.cpu_family == CPU_MMX_PENTIUM_FAMILY && np2cfg.cpu_model == CPU_MMX_PENTIUM_MODEL && !(np2cfg.cpu_feature_ex & CPU_FEATURE_EX_3DNOW)){
		strcpy(np2cfg.cpu_vendor, CPU_VENDOR_INTEL);
		strcpy(np2cfg.cpu_brandstring, CPU_BRAND_STRING_MMX_PENTIUM);
	}else if(np2cfg.cpu_family == CPU_PENTIUM_PRO_FAMILY && np2cfg.cpu_model == CPU_PENTIUM_PRO_MODEL && !(np2cfg.cpu_feature_ex & CPU_FEATURE_EX_3DNOW)){
		strcpy(np2cfg.cpu_vendor, CPU_VENDOR_INTEL);
		strcpy(np2cfg.cpu_brandstring, CPU_BRAND_STRING_PENTIUM_PRO);
	}else if(np2cfg.cpu_family == CPU_PENTIUM_II_FAMILY && np2cfg.cpu_model == CPU_PENTIUM_II_MODEL && !(np2cfg.cpu_feature_ex & CPU_FEATURE_EX_3DNOW)){
		strcpy(np2cfg.cpu_vendor, CPU_VENDOR_INTEL);
		strcpy(np2cfg.cpu_brandstring, CPU_BRAND_STRING_PENTIUM_II);
	}else if(np2cfg.cpu_family == CPU_PENTIUM_III_FAMILY && np2cfg.cpu_model == CPU_PENTIUM_III_MODEL && !(np2cfg.cpu_feature_ex & CPU_FEATURE_EX_3DNOW)){
		strcpy(np2cfg.cpu_vendor, CPU_VENDOR_INTEL);
		strcpy(np2cfg.cpu_brandstring, CPU_BRAND_STRING_PENTIUM_III);
	}else if(np2cfg.cpu_family == CPU_PENTIUM_M_FAMILY && np2cfg.cpu_model == CPU_PENTIUM_M_MODEL && !(np2cfg.cpu_feature_ex & CPU_FEATURE_EX_3DNOW)){
		strcpy(np2cfg.cpu_vendor, CPU_VENDOR_INTEL);
		strcpy(np2cfg.cpu_brandstring, CPU_BRAND_STRING_PENTIUM_M);
	}else if(np2cfg.cpu_family == CPU_PENTIUM_4_FAMILY && np2cfg.cpu_model == CPU_PENTIUM_4_MODEL && !(np2cfg.cpu_feature_ex & CPU_FEATURE_EX_3DNOW)){
		strcpy(np2cfg.cpu_vendor, CPU_VENDOR_INTEL);
		strcpy(np2cfg.cpu_brandstring, CPU_BRAND_STRING_PENTIUM_4);

	}else if(np2cfg.cpu_family == CPU_AMD_K6_2_FAMILY && np2cfg.cpu_model == CPU_AMD_K6_2_MODEL){
		strcpy(np2cfg.cpu_vendor, CPU_VENDOR_AMD);
		strcpy(np2cfg.cpu_brandstring, CPU_BRAND_STRING_AMD_K6_2);
	}else if(np2cfg.cpu_family == CPU_AMD_K6_III_FAMILY && np2cfg.cpu_model == CPU_AMD_K6_III_MODEL){
		strcpy(np2cfg.cpu_vendor, CPU_VENDOR_AMD);
		strcpy(np2cfg.cpu_brandstring, CPU_BRAND_STRING_AMD_K6_III);
	}else if(np2cfg.cpu_family == CPU_AMD_K7_ATHLON_FAMILY && np2cfg.cpu_model == CPU_AMD_K7_ATHLON_MODEL){
		strcpy(np2cfg.cpu_vendor, CPU_VENDOR_AMD);
		strcpy(np2cfg.cpu_brandstring, CPU_BRAND_STRING_AMD_K7_ATHLON);
	}else if(np2cfg.cpu_family == CPU_AMD_K7_ATHLON_XP_FAMILY && np2cfg.cpu_model == CPU_AMD_K7_ATHLON_XP_MODEL){
		strcpy(np2cfg.cpu_vendor, CPU_VENDOR_AMD);
		strcpy(np2cfg.cpu_brandstring, CPU_BRAND_STRING_AMD_K7_ATHLON_XP);

	}else if(np2cfg.cpu_family == 0 && np2cfg.cpu_model == 0 && np2cfg.cpu_stepping == 0 && np2cfg.cpu_feature == 0){
		strcpy(np2cfg.cpu_vendor, CPU_VENDOR_NEKOPRO);
		strcpy(np2cfg.cpu_brandstring, CPU_BRAND_STRING_NEKOPRO2);
	}else{
		strcpy(np2cfg.cpu_vendor, CPU_VENDOR_NEKOPRO);
		strcpy(np2cfg.cpu_brandstring, CPU_BRAND_STRING_NEKOPRO);
	}
#if defined(CPUCORE_IA32)
	if(strlen(np2cfg.cpu_vendor_o)!=0){
		memset(np2cfg.cpu_vendor, 0, 12);
#ifdef UNICODE
		WideCharToMultiByte(CP_ACP, 0, np2cfg.cpu_vendor_o, -1, np2cfg.cpu_vendor, 12+1, NULL, NULL);
#else
		strcpy(np2cfg.cpu_vendor, np2cfg.cpu_vendor_o);
#endif
		// 字数が足りない時スペースで埋める
		for(i=0;i<12;i++){
			if(np2cfg.cpu_vendor[i] == '\0'){
				np2cfg.cpu_vendor[i] = ' ';
			}
		}
		np2cfg.cpu_vendor[12] = '\0';
	}
	if(strlen(np2cfg.cpu_brandstring_o)!=0){
		memset(np2cfg.cpu_brandstring, 0, 48);
#ifdef UNICODE
		WideCharToMultiByte(CP_ACP, 0, np2cfg.cpu_brandstring_o, -1, np2cfg.cpu_brandstring, 48+1, NULL, NULL);
#else
		strcpy(np2cfg.cpu_brandstring, np2cfg.cpu_brandstring_o);
#endif
		// 最後に1文字スペースを入れる
		strcat(np2cfg.cpu_brandstring, " ");
	}
#endif
	strcpy(i386cpuid.cpu_vendor, np2cfg.cpu_vendor);
	if(np2cfg.cpu_family == 0 && np2cfg.cpu_model == 0 && np2cfg.cpu_stepping == 0 && np2cfg.cpu_feature == 0 && np2cfg.cpu_feature_ex == 0){
		// 設定に関係なく全部使えるようにする
		i386cpuid.cpu_family = CPU_FAMILY;
		i386cpuid.cpu_model = CPU_MODEL;
		i386cpuid.cpu_stepping = CPU_STEPPING;
		i386cpuid.cpu_feature = CPU_FEATURES_ALL;
		i386cpuid.cpu_feature_ex = CPU_FEATURES_EX_ALL;
		i386cpuid.cpu_feature_ecx = CPU_FEATURES_ALL;
		i386cpuid.cpu_brandid = 0;
	}else{
		i386cpuid.cpu_family = np2cfg.cpu_family;
		i386cpuid.cpu_model = np2cfg.cpu_model;
		i386cpuid.cpu_stepping = np2cfg.cpu_stepping;
		i386cpuid.cpu_feature = CPU_FEATURES_ALL & np2cfg.cpu_feature;
		i386cpuid.cpu_feature_ex = CPU_FEATURES_EX_ALL & np2cfg.cpu_feature_ex;
		i386cpuid.cpu_feature_ecx = CPU_FEATURES_ECX_ALL & np2cfg.cpu_feature_ecx;
		i386cpuid.cpu_brandid = np2cfg.cpu_brandid;
	}
	strcpy(i386cpuid.cpu_brandstring, np2cfg.cpu_brandstring);

	// BrandID自動設定（過去バージョンとの互換維持用）
	if(i386cpuid.cpu_brandid == CPU_BRAND_ID_AUTO){
		if(strncmp(i386cpuid.cpu_brandstring, CPU_BRAND_STRING_PENTIUM_III, 27)==0){
			CPU_EBX = 0x2;
		}else if(strncmp(i386cpuid.cpu_brandstring, CPU_BRAND_STRING_PENTIUM_M, 27)==0){
			CPU_EBX = 0x16;
		}else if(strncmp(i386cpuid.cpu_brandstring, CPU_BRAND_STRING_PENTIUM_4, 27)==0){
			CPU_EBX = 0x9;
		}else{
			CPU_EBX = 0;
		}
	}

	// FPU種類を設定
	i386cpuid.fpu_type = np2cfg.fpu_type;
	fpu_initialize();
#endif

	pccore_set(&np2cfg);
#if defined(SUPPORT_BMS)
	bmsio_set();
#endif
	nevent_allreset();

#if defined(VAEG_FIX)
	//後ろに移動
#else
	CPU_RESET();
	CPU_SETEXTSIZE((UINT32)pccore.extmem);
#endif

	CPU_TYPE = 0;
	if (pccore.dipsw[2] & 0x80) {
		CPU_TYPE = CPUTYPE_V30;
	}

#if defined(VAEG_FIX)
	CPU_RESET();
	CPU_SETEXTSIZE((UINT32)pccore.extmem);
#endif

	epson = (pccore.model & PCMODEL_EPSON) ? TRUE : FALSE;
	if (epson || np2cfg.useram_d) {
		/* enable RAM (D0000-DFFFF) */
		CPU_RAM_D000 = 0xffff;
	}
	font_setchargraph(epson);

	// HDDセット
	diskdrv_hddbind();
	// SASI/IDEどっち？
#if defined(SUPPORT_SASI)
	if (sxsi_issasi()) {
		pccore.hddif &= ~PCHDD_IDE;
		pccore.hddif |= PCHDD_SASI;
		TRACEOUT(("supported SASI"));
	}
#endif
#if defined(SUPPORT_SCSI)
	if (sxsi_isscsi()) {
		pccore.hddif |= PCHDD_SCSI;
		TRACEOUT(("supported SCSI"));
	}
#endif

	sound_changeclock();
	beep_changeclock();
	sound_reset();
	fddmtrsnd_bind();
	wabrlysnd_bind();

	fddfile_reset2dmode();
	bios0x18_16(0x20, 0xe1);

	iocore_reset(&np2cfg);								// サウンドでpicを呼ぶので…
	cbuscore_reset(&np2cfg);
	fmboard_reset(&np2cfg, pccore.sound);

	MEMM_ARCH((epson) ? 1 : 0);
	iocore_build();
	iocore_bind();
	cbuscore_bind();
	fmboard_bind();

	fddmtr_initialize();
	wabrly_initialize();
	calendar_initialize();
	vram_initialize();

	pal_change(1);

	bios_initialize();
	
	CS_BASE = 0xf0000;
	CPU_CS = 0xf000;
	CPU_IP = 0xfff0;

	CPU_CLEARPREFETCH();
	sysmng_cpureset();

#if defined(SUPPORT_HOSTDRV)
	hostdrv_reset();
#endif

	timing_reset();
	soundmng_play();

#ifdef SUPPORT_ASYNC_CPU
#if !defined(__LIBRETRO__) && !defined(NP2_SDL2) && !defined(NP2_X11)
	if(GetTickCounterMode()==TCMODE_PERFORMANCECOUNTER){
		asynccpu_clockpersec = GetTickCounter_ClockPerSec();
		asynccpu_lastclock = GetTickCounter_Clock();
		asynccpu_clockcount = GetTickCounter_Clock();
	}else{
		asynccpu_clockpersec.QuadPart = 0;
	}
#elif defined(NP2_X11) || defined(__LIBRETRO__)
	{
<<<<<<< HEAD
		asynccpu_lastclock = asynccpu_clockcount = (UINT64)clock();
=======
		struct timespec t;
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t);
		asynccpu_lastclock = asynccpu_clockcount = (UINT64)t.tv_sec * 1000000000 + t.tv_nsec;
>>>>>>> 6d2b0a232f47d9f3f0fff3496e1ec2a606564962
		asynccpu_clockpersec = CLOCKS_PER_SEC;
	}
#elif defined(NP2_SDL2)
	{
		asynccpu_lastclock = asynccpu_clockcount = SDL_GetPerformanceCounter();
		asynccpu_clockpersec = SDL_GetPerformanceFrequency();
	}
#endif
#endif
}

static void drawscreen(void) {

	UINT8	timing;
	void	(VRAMCALL * grphfn)(int page, int alldraw);
	UINT8	bit;
	

	tramflag.timing++;
	timing = ((LOADINTELWORD(gdc.m.para + GDC_CSRFORM + 1)) >> 5) & 0x3e;
	if (!timing) {
		timing = 0x40;
	}
	if (tramflag.timing >= timing) {
		tramflag.timing = 0;
		tramflag.count++;
		tramflag.renewal |= (tramflag.count ^ 2) & 2;
		tramflag.renewal |= 1;
	}

	if (gdcs.textdisp & GDCSCRN_EXT) {
		gdc_updateclock();
	}

	if (!pcstat.drawframe) {
		return;
	}
#ifdef SUPPORT_WAB
	if(np2wab.relay & 0x3){
		if(!np2wabcfg.multiwindow){
			pcstat.screenupdate = scrndraw_draw((UINT8)(pcstat.screenupdate & 2));
			drawcount++;
			return;
		}else{
			pcstat.screenupdate = 1;
			drawcount++;
		}
	}
#endif
	if ((gdcs.textdisp & GDCSCRN_EXT) || (gdcs.grphdisp & GDCSCRN_EXT)) {
		if (dispsync_renewalvertical()) {
			gdcs.textdisp |= GDCSCRN_ALLDRAW2;
			gdcs.grphdisp |= GDCSCRN_ALLDRAW2;
		}
	}
	if (gdcs.textdisp & GDCSCRN_EXT) {
		gdcs.textdisp &= ~GDCSCRN_EXT;
		dispsync_renewalhorizontal();
		tramflag.renewal |= 1;
		if (dispsync_renewalmode()) {
			pcstat.screenupdate |= 2;
		}
	}
	if (gdcs.palchange) {
		gdcs.palchange = 0;
		pal_change(0);
		pcstat.screenupdate |= 1;
	}
	if (gdcs.grphdisp & GDCSCRN_EXT) {
		gdcs.grphdisp &= ~GDCSCRN_EXT;
		if (((gdc.clock & 0x80) && (gdc.clock != 0x83)) ||
			(gdc.clock == 0x03)) {
			gdc.clock ^= 0x80;
			gdcs.grphdisp |= GDCSCRN_ALLDRAW2;
		}
	}
	if (gdcs.grphdisp & GDCSCRN_ENABLE) {
		if (!(gdc.mode1 & 2)) {
			grphfn = makegrph;
			bit = GDCSCRN_MAKE;
			if (gdcs.disp) {
				bit <<= 1;
			}
#if defined(SUPPORT_PC9821)
			if (gdc.analog & 2) {
				grphfn = makegrphex;
				if (gdc.analog & 4) {
					bit = GDCSCRN_MAKE | (GDCSCRN_MAKE << 1);
				}
			}
#endif
			if (gdcs.grphdisp & bit) {
				(*grphfn)(gdcs.disp, gdcs.grphdisp & bit & GDCSCRN_ALLDRAW2);
				gdcs.grphdisp &= ~bit;
				pcstat.screenupdate |= 1;
			}
		}
		else if (gdcs.textdisp & GDCSCRN_ENABLE) {
			if (!gdcs.disp) {
				if ((gdcs.grphdisp & GDCSCRN_MAKE) ||
					(gdcs.textdisp & GDCSCRN_MAKE)) {
					if (!(gdc.mode1 & 0x4)) {
						maketextgrph(0, gdcs.textdisp & GDCSCRN_ALLDRAW,
								gdcs.grphdisp & GDCSCRN_ALLDRAW);
					}
					else {
						maketextgrph40(0, gdcs.textdisp & GDCSCRN_ALLDRAW,
								gdcs.grphdisp & GDCSCRN_ALLDRAW);
					}
					gdcs.grphdisp &= ~GDCSCRN_MAKE;
					pcstat.screenupdate |= 1;
				}
			}
			else {
				if ((gdcs.grphdisp & (GDCSCRN_MAKE << 1)) ||
					(gdcs.textdisp & GDCSCRN_MAKE)) {
					if (!(gdc.mode1 & 0x4)) {
						maketextgrph(1, gdcs.textdisp & GDCSCRN_ALLDRAW,
								gdcs.grphdisp & (GDCSCRN_ALLDRAW << 1));
					}
					else {
						maketextgrph40(1, gdcs.textdisp & GDCSCRN_ALLDRAW,
								gdcs.grphdisp & (GDCSCRN_ALLDRAW << 1));
					}
					gdcs.grphdisp &= ~(GDCSCRN_MAKE << 1);
					pcstat.screenupdate |= 1;
				}
			}
		}
	}
	if (gdcs.textdisp & GDCSCRN_ENABLE) {
		if (tramflag.renewal) {
			gdcs.textdisp |= maketext_curblink();
		}
		if ((cgwindow.writable & 0x80) && (tramflag.gaiji)) {
			gdcs.textdisp |= GDCSCRN_ALLDRAW;
		}
		cgwindow.writable &= ~0x80;
		if (gdcs.textdisp & GDCSCRN_MAKE) {
			if (!(gdc.mode1 & 0x4)) {
				maketext(gdcs.textdisp & GDCSCRN_ALLDRAW);
			}
			else {
				maketext40(gdcs.textdisp & GDCSCRN_ALLDRAW);
			}
			gdcs.textdisp &= ~GDCSCRN_MAKE;
			pcstat.screenupdate |= 1;
		}
	}
	if (pcstat.screenupdate) {
		pcstat.screenupdate = scrndraw_draw((UINT8)(pcstat.screenupdate & 2));
		drawcount++;
	}
}

void screendisp(NEVENTITEM item) {

	PICITEM		pi;

	gdc_work(GDCWORK_SLAVE);
	gdc.vsync = 0;
	pcstat.screendispflag = 0;
	if (!np2cfg.DISPSYNC) {
		drawscreen();
	}
	pi = &pic.pi[0];
	if (pi->irr & PIC_CRTV) {
		pi->irr &= ~PIC_CRTV;
		gdc.vsyncint = 1;
	}
	(void)item;
}

void screenvsync(NEVENTITEM item) {

	MEMWAIT_TRAM = np2cfg.wait[1];
	MEMWAIT_VRAM = np2cfg.wait[3];
	MEMWAIT_GRCG = np2cfg.wait[5];
	gdc_work(GDCWORK_MASTER);
	gdc.vsync = 0x20;
	if (gdc.vsyncint) {
		gdc.vsyncint = 0;
		pic_setirq(2);
	}
	nevent_set(NEVENT_FLAMES, gdc.vsyncclock, screendisp, NEVENT_RELATIVE);

	// drawscreenで pccore.vsyncclockが変更される可能性があります
	if (np2cfg.DISPSYNC) {
		drawscreen();
	}
	(void)item;
}


// ---------------------------------------------------------------------------

// #define SINGLESTEPONLY

#if defined(TRACE)
static int resetcnt = 0;
static int execcnt = 0;
int piccnt = 0;
#endif

void pccore_postevent(UINT32 event) {	// yet!

	(void)event;
}

void pccore_exec(BOOL draw) {

	static UINT32 disptmr = 0;

	pcstat.drawframe = (UINT8)draw;
//	keystat_sync();
	soundmng_sync();
	mouseif_sync();
	pal_eventclear();

	gdc.vsync = 0;
	pcstat.screendispflag = 1;
	MEMWAIT_TRAM = np2cfg.wait[0];
	MEMWAIT_VRAM = np2cfg.wait[2];
	MEMWAIT_GRCG = np2cfg.wait[4];
	
	nevent_set(NEVENT_FLAMES, gdc.dispclock, screenvsync, NEVENT_RELATIVE);

//	nevent_get1stevent();
	
	while(pcstat.screendispflag) {
#if defined(TRACE)
		resetcnt++;
#endif
		pic_irq();
		if (CPU_RESETREQ) {
			CPU_RESETREQ = 0;
#if defined(SUPPORT_WAB)
			np2wab.relaystateint = np2wab.relaystateext = 0;
			np2wab_setRelayState(0); // XXX:
#endif
#if defined(SUPPORT_CL_GD5430)
			np2clvga.gd54xxtype = np2clvga.defgd54xxtype; // Auto Select用
			pc98_cirrus_vga_resetresolution();
#endif
#if defined(SUPPORT_IDEIO)
			ideio_basereset(); // XXX: Win9xの再起動で必要
#endif
#if defined(SUPPORT_HOSTDRV)
			hostdrv_reset(); // XXX: Win9xの再起動で必要
#endif
#if defined(SUPPORT_PCI)
			pcidev_basereset(); // XXX: Win9xの再起動で必要
#endif
			CPU_SHUT();
		}
#if defined(USE_TSC)
#if defined(NP2_X11) || defined(NP2_SDL2) || defined(__LIBRETRO__)
		CPU_MSR_TSC += CPU_BASECLOCK;//CPU_REMCLOCK;
#endif
#endif
#if !defined(SINGLESTEPONLY)
		if (CPU_REMCLOCK > 0) {
			if (!(CPU_TYPE & CPUTYPE_V30)) {
				CPU_EXEC();
			}
			else {
				CPU_EXECV30();
			}
		}
#else
		while(CPU_REMCLOCK > 0) {
			CPU_STEPEXEC();
		}
#endif
#if defined(SUPPORT_HRTIMER)
	upd4990_hrtimer_count();
#endif	/* SUPPORT_HRTIMER */
		nevent_progress();
	}
	artic_callback();
	mpu98ii_callback();
	diskdrv_callback();
	calendar_inc();
	S98_sync();
	sound_sync();
	fdc_intdelay();	// FDC SEEK & RECALIBRATE, etc. np21w ver0.86 rev46

	if (pcstat.hardwarereset) {
		pcstat.hardwarereset = FALSE;
		pccore_cfgupdate();
		pccore_reset();
	}

#if defined(TRACE)
	execcnt++;
	if (execcnt >= 60) {
//		TRACEOUT(("resetcnt = %d / pic %d", resetcnt, piccnt));
		execcnt = 0;
		resetcnt = 0;
		piccnt = 0;
	}
#endif
}

