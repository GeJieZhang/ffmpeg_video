#include "test.h"
#include<android/log.h>
#include <string>

#define TAG    "include.ffmpeg=======" // 这个是自定义的LOG的标识
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__) // 定义LOGD类型
extern "C"
{
#include <include/libavformat/avformat.h>
};

void log_ss(const char *msg, int d = -1123) {
    if (d == -1123) {
        LOGE("%s\n", msg);
    } else {
        LOGE("%s  %d \n", msg, d);
    }
}

void test::main(const char *str, const char *str2) {
    ///LOGE("你好");
    init(str, str2);
}

 test::test(JNIEnv *env2) {

    env = env2;

}

 test::~test() {

    env = NULL;

    delete env;

}


void test::init(const char *str, const char *str2) {
    AVOutputFormat *ofmt = NULL;
    //Input AVFormatContext and Output AVFormatContext
    AVFormatContext *ifmt_ctx = NULL, *ofmt_ctx = NULL;
    AVPacket pkt;
    int ret, i,code;
    int frame_index = 0;


    const char *in_filename = str;//Input file URL
    const char *out_filename = str2;//Output file URL
//    printf("%s",str);
//    printf("%s",str2);


    LOGE("%s\n", in_filename);
    //LOGE(in_filename);
    //LOGE(env->NewStringUTF(out_filename));

    //1. 注册初始化
    //H.264 bitstream malformed, no startcode found, use the video bitstream filte错误解决方法   第一步
    AVBitStreamFilterContext *filter = av_bitstream_filter_init("h264_mp4toannexb");
    if (NULL == filter) {
        log_ss("filter init fail");
    }
    //av_register_bitstream_filter(filter->filter);
    av_register_all();
    //2. 打开媒体文件与AVFormatContext关联
    //Input
    if ((ret = avformat_open_input(&ifmt_ctx, in_filename, 0, 0)) < 0) {
        log_ss("Could not open input file.");

        log_ss("failed",ret);

        goto end;
    }
    //3. 获取视频流信息
    if ((ret = avformat_find_stream_info(ifmt_ctx, 0)) < 0) {//
        log_ss("Failed to retrieve input stream information");
        goto end;
    }
    log_ss("--------------- In File Information ----------------");
    av_dump_format(ifmt_ctx, 0, in_filename, 0);
    log_ss("--------------- In File Information ----------------");
    //4. 初始化输出视频码流的AVFormatContext，与输出文件相关联
    avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, out_filename);
    if (!ofmt_ctx) {
        log_ss("Could not create output context");
        ret = AVERROR_UNKNOWN;
        goto end;
    }

    //获取AVOutputFormat
    ofmt = ofmt_ctx->oformat;
    for (i = 0; i < ifmt_ctx->nb_streams; i++) {
        //Create output AVStream according to input AVStream
        AVStream *in_stream = ifmt_ctx->streams[i];
        AVStream *out_stream = avformat_new_stream(ofmt_ctx,
                                                   in_stream->codec->codec);// 初始化AVStream
        if (!out_stream) {
            log_ss("Failed allocating output stream\n");
            ret = AVERROR_UNKNOWN;
            goto end;
        }
        //5.【关键步骤】 copy输入文件的设置到输出文件
        if (avcodec_copy_context(out_stream->codec, in_stream->codec) < 0) {
            log_ss("Failed to copy context from input to output stream codec context\n");
            goto end;
        }
        out_stream->codec->codec_tag = 0;
        if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
            out_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
    }
    log_ss("--------------- Out File Information ----------------");
    av_dump_format(ofmt_ctx, 0, out_filename, 1);
    log_ss("--------------- Out File Information ----------------");
    //6. 打开输出文件。
    if (!(ofmt->flags & AVFMT_NOFILE)) {
        ret = avio_open(&ofmt_ctx->pb, out_filename, AVIO_FLAG_WRITE);
        if (ret < 0) {
            log_ss("Could not open output file");
            log_ss(out_filename);
            goto end;
        }
    }
    //7. 写入头部到文件
    if (avformat_write_header(ofmt_ctx, NULL) < 0) {
        log_ss("Error occurred when opening output file\n");
        goto end;
    }

    //8. 写入数据到文件
    while (1) {
        AVStream *in_stream, *out_stream;
        //Get an AVPacket
        ret = av_read_frame(ifmt_ctx, &pkt);
        if (ret < 0)
            break;
        in_stream = ifmt_ctx->streams[pkt.stream_index];
        out_stream = ofmt_ctx->streams[pkt.stream_index];

        //H.264 bitstream malformed, no startcode found, use the video bitstream filte错误解决方法   第二步
        if (pkt.stream_index == 0) {
            AVPacket fpkt = pkt;
            int a = av_bitstream_filter_filter(filter,
                                               out_stream->codec, NULL, &fpkt.data, &fpkt.size,
                                               pkt.data, pkt.size, pkt.flags & AV_PKT_FLAG_KEY);
            pkt.data = fpkt.data;
            pkt.size = fpkt.size;
        }

        //Convert PTS/DTS
        pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base,
                                   (AVRounding) (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
        pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base,
                                   (AVRounding) (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
        pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
        pkt.pos = -1;
        //将AVPacket（存储视频压缩码流数据）写入文件   av_interleaved_write_frame将对 packet 进行缓存和 pts 检查，av_write_frame没有
        if (av_interleaved_write_frame(ofmt_ctx, &pkt) < 0) {
            log_ss("Error muxing packets",code);


            break;
        }


        log_ss("Write frames to output file:", frame_index);
        av_packet_unref(&pkt);
        frame_index++;
    }

    //9. 写入文件尾部
    //Write file trailer
    av_write_trailer(ofmt_ctx);
    end:
    avformat_close_input(&ifmt_ctx);
    /* close output */
    if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE))
        avio_close(ofmt_ctx->pb);
    avformat_free_context(ofmt_ctx);

    getchar();
    LOGE("视频转换完毕！");

}