#ifndef _BASE_LOG_H_
#define _BASE_LOG_H_

#include <stdio.h>
#include <time.h>
#include <sys/time.h>

#ifdef MULITTHREADLOG
#include <base/util/mutex.h>
#endif


namespace base
{

    #ifndef MAX_PATH
    /// ֧�ֵ�����ļ�·������
    #define MAX_PATH    1024
    #endif

    /// ��־����
    enum LogLevel
    {
	    L_FATAL = 0,    ///< �������󣬳��򲻵ò�ֹͣ
	    L_ERROR,        ///< ���ϣ����������Ӵ���
	    L_WARN,         ///< ���棬��Ӱ��ҵ��Ĺ���
	    L_INFO,         ///< ҵ����Ϣ��¼
//		    L_RESERVD,      ///< �������̸���
	    L_DEBUG,        ///< ������Ϣ
		L_TRACE,		///< ��������ϸ��Ϣ
        L_LEVEL_MAX
    };

    /**
        ��־��

        
        ��־�ļ�����ʽ��
            - xxx-20081105-090000.log

        ��־�ļ��ָ
            - �µ�һ�쿪ʼ�������ļ���С�����趨ֵ�������ļ�д��

        �÷���
            <CODE>
            #include <log.h>

            LOG_INIT("xxx", 400000, L_TRACE);

            LOG_OPEN();

            LOG(3)("warn info %d", 3);

            LOG_CLOSE();

            </CODE>
        */
    class Log
    {
    public:
        /**
            ����Log����
        */
	    Log();

        /**
            ����Log����
            
            ����ǰ���ر���־�ļ�
            @see close()
        */
	    ~Log();

    public:
        /**
            ������־�ļ���
            
            ����Ѿ����˾Ͳ�����������
            @param filename �µ���־�ļ���
            @return 0 �ɹ� -1 ���ִ���
        */
	    int set_file_name(const char *filename);

        /**
            ������־�ļ���ģ����
            
            ��������Ч�����´������־ʱ�ż��
            @param maxsize �µĹ�ģ����
            @return 0 �ɹ� -1 ���ִ���
        */
	    int set_max_size(size_t maxsize);

        /**
            ���������־�ȼ�         
              
            ֻ�Ժ����������־��Ч
            @param level �µļ�������
            @return 0 �ɹ� -1 ���ִ���
        */
	    int set_max_level(LogLevel level);

		/**
			�Ƿ������ usec����
		**/
		int set_usec(bool in_enable_usec);

		/**
			�Ƿ��16����pack��ӡ���ܣ�Ĭ��Ϊ��
		**/
		int set_pack_print(bool in_enable_pack_print);


        /**
            ��ȡ��־�ļ���ģ����
            
            @return ��־�ļ���ģ����
        */
	    size_t get_max_size() {return max_size_;}

        /**
            ��ȡ�����־�ȼ�           
            
            @return �����־�ȼ�
        */
        LogLevel get_max_level() {return max_level_;}

        /**
            ����־�ļ�

            ��ǰ���������ļ���
            @return 0 �ɹ� -1 ���ִ���
            @see set_file_name()
        */
	    int open();

        /**
            �ر���־�ļ�

            �رպ���������־�ļ�
            @return 0 �ɹ� -1 ���ִ���
            @see set_file_name()
        */
	    int close();

    #ifdef WIN32	// for windows
    #define CHECK_FORMAT(i, j)
    #else			// for linux(gcc)
    #define CHECK_FORMAT(i, j) __attribute__((format(printf, i, j)))
    #endif

        /**
            ���һ����־��¼

            @param level ��־�ȼ�
            @param fmt ��ʽ���ַ���
            @return 0 �ɹ� -1 ���ִ���
        */
        int log(LogLevel level, const char * fmt, ...) CHECK_FORMAT(3, 4);

        /// ���һ��FATAL��־��¼
	    int log_fatal(const char * fmt, ...) CHECK_FORMAT(2, 3);

        /// ���һ��ERROR��־��¼
        int log_error(const char * fmt, ...) CHECK_FORMAT(2, 3);

        /// ���һ��WARN��־��¼
        int log_warn(const char * fmt, ...) CHECK_FORMAT(2, 3);

        /// ���һ��INFO��־��¼
        int log_info(const char * fmt, ...) CHECK_FORMAT(2, 3);

        /// ���һ��TRACE��־��¼
        int log_trace(const char * fmt, ...) CHECK_FORMAT(2, 3);

        /// ���һ��DEBUG��־��¼
        int log_debug(const char * fmt, ...) CHECK_FORMAT(2, 3);

    #undef CHECK_FORMAT

        /**
            ��ʮ������dumpһ������

            @param data �����׵�ַ
            @param len ���ݳ���
            @param level ��־�ȼ�
            @return 0 �ɹ� -1 ���ִ���
        */
        int log_hex(unsigned char * data, size_t len, LogLevel level);
		int log_hex_prefix(unsigned char * prefix, unsigned char * data, size_t len, LogLevel level);

    public:
        /// ȫ����־����
        static Log global_log;

    private:
        class Lock;

        /**
            ���һ����־��¼

            @param level ��־�ȼ�
            @param fmt ��ʽ���ַ���
            @param ap ��ʽ������
            @return 0 �ɹ� -1 ���ִ���
            @see log()
        */
	    int vlog(int level, const char* fmt, va_list ap);

    private:
	    /// ��ͬ��־�������ɫ�Լ��ؼ�������
	    static char level_str_[L_LEVEL_MAX][64];		
	    static char level_str_usec_[L_LEVEL_MAX][64];

    private:
	    /// ��־�ļ���
	    char file_name_[MAX_PATH];

	    /// ������־�ļ�����ļ���С
	    size_t max_size_;

        /// ��־����
        LogLevel max_level_;

	    /// ��־�ļ��ļ�������
	    FILE * file_;

#ifdef MULITTHREADLOG
	    /// �߳���
        base::IMutex * lock_;
#endif

	    /// ���쿪ʼʱ��
        time_t mid_night_;

		bool enable_usec;

		bool enable_pack_print;
    };

}

#define LOG(level) LOG_##level

#define LOG_0 LOG_FATAL
#define LOG_1 LOG_ERROR
#define LOG_2 LOG_WARN
#define LOG_3 LOG_INFO
#define LOG_4 LOG_DEBUG
#define LOG_5 LOG_TRACE


#define LOG_FATAL base::Log::global_log.log_fatal
#define LOG_ERROR base::Log::global_log.log_error
#define LOG_WARN  base::Log::global_log.log_warn
#define LOG_INFO  base::Log::global_log.log_info
#define LOG_TRACE base::Log::global_log.log_trace
#define LOG_DEBUG base::Log::global_log.log_debug

#define LOG_HEX(_context, _len, level) base::Log::global_log.log_hex((unsigned char *)(_context), (_len), (level))
#define LOG_HEX_PREFIX(prefix, _context, _len, level) base::Log::global_log.log_hex_prefix((unsigned char *)(prefix), (unsigned char *)(_context), (_len), (level))


#define LOG_INIT(name, size, level) \
    base::Log::global_log.set_file_name(name); \
    base::Log::global_log.set_max_size(size); \
    base::Log::global_log.set_max_level((base::LogLevel)(level))

#define LOG_OPEN() base::Log::global_log.open()

#define LOG_CLOSE() base::Log::global_log.close()

#define LOG_SET_LEVEL(level) base::Log::global_log.set_max_level((base::LogLevel)(level))

#define LOG_GET_LEVEL() base::Log::global_log.get_max_level()

#define LOG_SET_USEC(enable_usec) base::Log::global_log.set_usec(enable_usec)

#define LOG_SET_PACK_PRINT(in_enable_pack_print) base::Log::global_log.set_pack_print(in_enable_pack_print)


#endif
