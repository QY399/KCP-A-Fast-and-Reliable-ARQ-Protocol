//=====================================================================
//
// KCP
//  
// Features:
// + Average RTT reduce 30% - 40% vs traditional ARQ like tcp.
// + Maximum RTT reduce three times vs tcp.
// + Lightweight, distributed as a single source file.
//
//=====================================================================
#ifndef __IKCP_H__
#define __IKCP_H__

#include <stddef.h>
#include <stdlib.h>
#include <assert.h>


//=====================================================================
// 32BIT INTEGER DEFINITION 
//=====================================================================
#ifndef __INTEGER_32_BITS__
#define __INTEGER_32_BITS__
#if defined(_WIN64) || defined(WIN64) || defined(__amd64__) || \
	defined(__x86_64) || defined(__x86_64__) || defined(_M_IA64) || \
	defined(_M_AMD64)
typedef unsigned int ISTDUINT32;
typedef int ISTDINT32;
#elif defined(_WIN32) || defined(WIN32) || defined(__i386__) || \
	defined(__i386) || defined(_M_X86)
typedef unsigned long ISTDUINT32;
typedef long ISTDINT32;
#elif defined(__MACOS__)
typedef UInt32 ISTDUINT32;
typedef SInt32 ISTDINT32;
#elif defined(__APPLE__) && defined(__MACH__)
#include <sys/types.h>
typedef u_int32_t ISTDUINT32;
typedef int32_t ISTDINT32;
#elif defined(__BEOS__)
#include <sys/inttypes.h>
typedef u_int32_t ISTDUINT32;
typedef int32_t ISTDINT32;
#elif (defined(_MSC_VER) || defined(__BORLANDC__)) && (!defined(__MSDOS__))
typedef unsigned __int32 ISTDUINT32;
typedef __int32 ISTDINT32;
#elif defined(__GNUC__)
#include <stdint.h>
typedef uint32_t ISTDUINT32;
typedef int32_t ISTDINT32;
#else 
typedef unsigned long ISTDUINT32;
typedef long ISTDINT32;
#endif
#endif


//=====================================================================
// Integer Definition
//=====================================================================
#ifndef __IINT8_DEFINED
#define __IINT8_DEFINED
typedef char IINT8;
#endif

#ifndef __IUINT8_DEFINED
#define __IUINT8_DEFINED
typedef unsigned char IUINT8;
#endif

#ifndef __IUINT16_DEFINED
#define __IUINT16_DEFINED
typedef unsigned short IUINT16;
#endif

#ifndef __IINT16_DEFINED
#define __IINT16_DEFINED
typedef short IINT16;
#endif

#ifndef __IINT32_DEFINED
#define __IINT32_DEFINED
typedef ISTDINT32 IINT32;
#endif

#ifndef __IUINT32_DEFINED
#define __IUINT32_DEFINED
typedef ISTDUINT32 IUINT32;
#endif

#ifndef __IINT64_DEFINED
#define __IINT64_DEFINED
#if defined(_MSC_VER) || defined(__BORLANDC__)
typedef __int64 IINT64;
#else
typedef long long IINT64;
#endif
#endif

#ifndef __IUINT64_DEFINED
#define __IUINT64_DEFINED
#if defined(_MSC_VER) || defined(__BORLANDC__)
typedef unsigned __int64 IUINT64;
#else
typedef unsigned long long IUINT64;
#endif
#endif

#ifndef INLINE
#if defined(__GNUC__)

#if (__GNUC__ > 3) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1))
#define INLINE         __inline__ __attribute__((always_inline))
#else
#define INLINE         __inline__
#endif

#elif (defined(_MSC_VER) || defined(__BORLANDC__) || defined(__WATCOMC__))
#define INLINE __inline
#else
#define INLINE 
#endif
#endif

#if (!defined(__cplusplus)) && (!defined(inline))
#define inline INLINE
#endif


//=====================================================================
// QUEUE DEFINITION                                                  
//=====================================================================
#ifndef __IQUEUE_DEF__
#define __IQUEUE_DEF__

struct IQUEUEHEAD {
	struct IQUEUEHEAD *next, *prev;
};

typedef struct IQUEUEHEAD iqueue_head;


//---------------------------------------------------------------------
// queue init                                                         
//---------------------------------------------------------------------
#define IQUEUE_HEAD_INIT(name) { &(name), &(name) }
#define IQUEUE_HEAD(name) \
struct IQUEUEHEAD name = IQUEUE_HEAD_INIT(name)

#define IQUEUE_INIT(ptr) ( \
	(ptr)->next = (ptr), (ptr)->prev = (ptr))

#define IOFFSETOF(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#define ICONTAINEROF(ptr, type, member) ( \
	(type*)(((char*)((type*)ptr)) - IOFFSETOF(type, member)))

#define IQUEUE_ENTRY(ptr, type, member) ICONTAINEROF(ptr, type, member)


//---------------------------------------------------------------------
// queue operation                     
//---------------------------------------------------------------------
#define IQUEUE_ADD(node, head) ( \
	(node)->prev = (head), (node)->next = (head)->next, \
	(head)->next->prev = (node), (head)->next = (node))

#define IQUEUE_ADD_TAIL(node, head) ( \
	(node)->prev = (head)->prev, (node)->next = (head), \
	(head)->prev->next = (node), (head)->prev = (node))

#define IQUEUE_DEL_BETWEEN(p, n) ((n)->prev = (p), (p)->next = (n))

#define IQUEUE_DEL(entry) (\
	(entry)->next->prev = (entry)->prev, \
	(entry)->prev->next = (entry)->next, \
	(entry)->next = 0, (entry)->prev = 0)

#define IQUEUE_DEL_INIT(entry) do { \
	IQUEUE_DEL(entry); IQUEUE_INIT(entry);} while (0)

#define IQUEUE_IS_EMPTY(entry) ((entry) == (entry)->next)

#define iqueue_init		IQUEUE_INIT
#define iqueue_entry	IQUEUE_ENTRY
#define iqueue_add		IQUEUE_ADD
#define iqueue_add_tail	IQUEUE_ADD_TAIL
#define iqueue_del		IQUEUE_DEL
#define iqueue_del_init	IQUEUE_DEL_INIT
#define iqueue_is_empty IQUEUE_IS_EMPTY

#define IQUEUE_FOREACH(iterator, head, TYPE, MEMBER) \
for ((iterator) = iqueue_entry((head)->next, TYPE, MEMBER); \
	&((iterator)->MEMBER) != (head); \
	(iterator) = iqueue_entry((iterator)->MEMBER.next, TYPE, MEMBER))

#define iqueue_foreach(iterator, head, TYPE, MEMBER) \
	IQUEUE_FOREACH(iterator, head, TYPE, MEMBER)

#define iqueue_foreach_entry(pos, head) \
for ((pos) = (head)->next; (pos) != (head); (pos) = (pos)->next)


#define __iqueue_splice(list, head) do {	\
	iqueue_head *first = (list)->next, *last = (list)->prev; \
	iqueue_head *at = (head)->next; \
	(first)->prev = (head), (head)->next = (first);		\
	(last)->next = (at), (at)->prev = (last);}	while (0)

#define iqueue_splice(list, head) do { \
	if (!iqueue_is_empty(list)) __iqueue_splice(list, head);} while (0)

#define iqueue_splice_init(list, head) do {	\
	iqueue_splice(list, head);	iqueue_init(list);} while (0)


#ifdef _MSC_VER
#pragma warning(disable:4311)
#pragma warning(disable:4312)
#pragma warning(disable:4996)
#endif

#endif


	//---------------------------------------------------------------------
	// BYTE ORDER & ALIGNMENT
	//---------------------------------------------------------------------
#ifndef IWORDS_BIG_ENDIAN
#ifdef _BIG_ENDIAN_
#if _BIG_ENDIAN_
#define IWORDS_BIG_ENDIAN 1
#endif
#endif
#ifndef IWORDS_BIG_ENDIAN
#if defined(__hppa__) || \
	defined(__m68k__) || defined(mc68000) || defined(_M_M68K) || \
	(defined(__MIPS__) && defined(__MIPSEB__)) || \
	defined(__ppc__) || defined(__POWERPC__) || defined(_M_PPC) || \
	defined(__sparc__) || defined(__powerpc__) || \
	defined(__mc68000__) || defined(__s390x__) || defined(__s390__)
#define IWORDS_BIG_ENDIAN 1
#endif
#endif
#ifndef IWORDS_BIG_ENDIAN
#define IWORDS_BIG_ENDIAN  0
#endif
#endif

#ifndef IWORDS_MUST_ALIGN
#if defined(__i386__) || defined(__i386) || defined(_i386_)
#define IWORDS_MUST_ALIGN 0
#elif defined(_M_IX86) || defined(_X86_) || defined(__x86_64__)
#define IWORDS_MUST_ALIGN 0
#elif defined(__amd64) || defined(__amd64__)
#define IWORDS_MUST_ALIGN 0
#else
#define IWORDS_MUST_ALIGN 1
#endif
#endif


	//=====================================================================
	// SEGMENT  段
	//=====================================================================
	struct IKCPSEG
	{
		struct IQUEUEHEAD node;
		IUINT32 conv;  //unsigned long
		IUINT32 cmd;
		IUINT32 frg;
		IUINT32 wnd;
		IUINT32 ts;
		IUINT32 sn;
		IUINT32 una;
		IUINT32 len;
		IUINT32 resendts;
		IUINT32 rto;
		IUINT32 fastack;
		IUINT32 xmit;
		char data[1];
	};


	//---------------------------------------------------------------------
	// IKCPCB
	//---------------------------------------------------------------------
	struct IKCPCB
	{
		IUINT32 conv, mtu, mss, state;
		IUINT32 snd_una, snd_nxt, rcv_nxt;
		IUINT32 ts_recent, ts_lastack, ssthresh;
		IINT32 rx_rttval, rx_srtt, rx_rto, rx_minrto;
		IUINT32 snd_wnd, rcv_wnd, rmt_wnd, cwnd, probe;
		IUINT32 current, interval, ts_flush, xmit;
		IUINT32 nrcv_buf, nsnd_buf;
		IUINT32 nrcv_que, nsnd_que;
		IUINT32 nodelay, updated;
		IUINT32 ts_probe, probe_wait;
		IUINT32 dead_link, incr;
		struct IQUEUEHEAD snd_queue;
		struct IQUEUEHEAD rcv_queue;
		struct IQUEUEHEAD snd_buf;
		struct IQUEUEHEAD rcv_buf;
		IUINT32 *acklist;
		IUINT32 ackcount;
		IUINT32 ackblock;
		void *user;
		char *buffer;
		int fastresend;
		int fastlimit;
		int nocwnd, stream;
		int logmask;
		int(*output)(const char *buf, int len, struct IKCPCB *kcp, void *user);
		void(*writelog)(const char *log, struct IKCPCB *kcp, void *user);
	};


	typedef struct IKCPCB ikcpcb;

#define IKCP_LOG_OUTPUT			1
#define IKCP_LOG_INPUT			2
#define IKCP_LOG_SEND			4
#define IKCP_LOG_RECV			8
#define IKCP_LOG_IN_DATA		16
#define IKCP_LOG_IN_ACK			32
#define IKCP_LOG_IN_PROBE		64
#define IKCP_LOG_IN_WINS		128
#define IKCP_LOG_OUT_DATA		256
#define IKCP_LOG_OUT_ACK		512
#define IKCP_LOG_OUT_PROBE		1024
#define IKCP_LOG_OUT_WINS		2048

#ifdef __cplusplus
	extern "C" {
#endif

		//---------------------------------------------------------------------
		// 接口
		//---------------------------------------------------------------------

		// 创建一个新的kcp控制对象，'conv'必须等于两个端点
		// 来自同一个连接。“user”将被传递到输出回调
		// 输出回调可以这样设置:'kcp->output = my_udp_output'
		ikcpcb* ikcp_create(IUINT32 conv, void *user);

		// 释放kcp控制对象
		void ikcp_release(ikcpcb *kcp);

		// 设置输出回调，它将被kcp调用
		void ikcp_setoutput(ikcpcb *kcp, int(*output)(const char *buf, int len,
			ikcpcb *kcp, void *user));

		// 用户/上层recv:返回大小，再次为EAGAIN返回小于零的值
		int ikcp_recv(ikcpcb *kcp, char *buffer, int len);

		// 用户/上层发送，返回0以下的错误
		int ikcp_send(ikcpcb *kcp, const char *buffer, int len);

		// 更新状态(每隔10ms-100ms重复调用一次)，或者您可以询问
		// ikcp_check何时再次调用它(不调用ikcp_input/_send)。
		// 'current' -当前时间戳millisec。
		void ikcp_update(ikcpcb *kcp, IUINT32 current);

		// 确定何时调用ikcp_update:
		// 如果没有ikcp_input/_send调用，则返回应该在millisec中
		// 调用ikcp_update的时间。您可以在那个时间调用ikcp_update，
		// 而不是重复调用update。
		//
		// 减少不必要的 ikcp_update调用非常重要。使用它来调度ikcp_update
		// (例如。实现类似于epoll的机制，或在处理大量kcp连接时优化ikcp_update) 
	
		
		IUINT32 ikcp_check(const ikcpcb *kcp, IUINT32 current);

		// 当您收到一个低级别数据包时(例如。UDP数据包)，调用它
		int ikcp_input(ikcpcb *kcp, const char *data, long size);

		// 冲洗未决数据
		void ikcp_flush(ikcpcb *kcp);

		// 检查recv队列中的下一条消息的大小
		int ikcp_peeksize(const ikcpcb *kcp);

		// 更改MTU大小，默认为1400
		int ikcp_setmtu(ikcpcb *kcp, int mtu);

		// 设置最大窗口大小:sndwnd=32，默认rcvwnd=32
		int ikcp_wndsize(ikcpcb *kcp, int sndwnd, int rcvwnd);

		// 获取等待发送的数据包数量
		int ikcp_waitsnd(const ikcpcb *kcp);

		// 最快:ikcp_nodelay(kcp, 1,20,2,1)
		// nodelay: 0:禁用(默认),1:启用
		// interval:在millisec中，内部更新计时器间隔，默认为100ms
		// resend: 0:禁用fast resend(默认值)，1:启用fast resend
		// nc: 0:正常拥塞控制(默认值)，1:禁用拥塞控制
		int ikcp_nodelay(ikcpcb *kcp, int nodelay, int interval, int resend, int nc);


		void ikcp_log(ikcpcb *kcp, int mask, const char *fmt, ...);

		// 设置分配器
		void ikcp_allocator(void* (*new_malloc)(size_t), void(*new_free)(void*));

		// 读取 conv
		IUINT32 ikcp_getconv(const void *ptr);


#ifdef __cplusplus
	}
#endif

#endif

