#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
static void fill_input(uint8_t *in_stream)
{
    in_stream[0] = 00;
    in_stream[1] = 00;
    in_stream[2] = 01;
    in_stream[3] = 0xDE;
    in_stream[4] = 0xAD;
    in_stream[5] = 0xBE;
    in_stream[6] = 0xEF;
    in_stream[7] = 0x01;
    in_stream[8] = 0x23;
    in_stream[9] = 0x00;
    in_stream[10] = 0x01;
    in_stream[11] = 0x89;
    in_stream[12] = 0x00;
    in_stream[13] = 0x00;
    in_stream[14] = 0x00;
    in_stream[15] = 0x00;
    in_stream[16] = 0x01;
    in_stream[17] = 0xDA;
    in_stream[18] = 0xDB;
    in_stream[19] = 0xAD;
    in_stream[20] = 0xBF;
    in_stream[21] = 0xBA;
    in_stream[22] = 0xDA;
    in_stream[23] = 0xDB;
    in_stream[24] = 0xAD;
    in_stream[25] = 0xBF;
    in_stream[26] = 0xBA;
    in_stream[27] = 0xDA;
    in_stream[28] = 0xDB;
    in_stream[29] = 0xAD;
    in_stream[30] = 0xBF;
    in_stream[31] = 0x00;
    in_stream[32] = 0xDA;
    in_stream[33] = 0x00;
    in_stream[34] = 0x01;
    in_stream[35] = 0xBF;
    in_stream[36] = 0xBA;
    in_stream[37] = 00;
    in_stream[38] = 00;
    in_stream[39] = 01;
    in_stream[40] = 0xAA;
}
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
static void print_stream(uint8_t *stream, uint32_t stream_len)
{
    int32_t i;
    printf("Printing stream in \"box\" format\n");
    for (i = 0; i < stream_len; i++)
        printf("%x\t", stream[i]);
}
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
static void convert_nal_units(uint8_t *in_stream, uint32_t in_len, uint8_t *out_stream, uint32_t out_len)
{
    uint32_t i = 0;
    uint32_t j = 0;
    int32_t start_flag = 0;
    int32_t start_pos = -1;
    int32_t count = 0;
    int32_t num_nals = 0;
    int32_t prev_start = -1;
    int32_t prev_end = -1;
    int32_t out_start = 0;
    int32_t output_nals = 0;
    for (i = 0; i < in_len; i++)
    {
        if (in_stream[i] == 0x00 && start_flag == 0)
        {
            start_flag = 1;
            start_pos = i;
        }
        if (in_stream[i] == 0 && start_flag == 1)
            count++;
        if (in_stream[i] == 1 && count > 1)
        {
            num_nals++;
            count++;
            // Copy bytes after second start code is detected in a stream
            if (num_nals > 1)
            {
                int32_t prev_len = start_pos - (prev_end + 1);
                out_stream[out_start + 3] = prev_len & 0xFF;
                out_stream[out_start + 2] = (prev_len >> 8) & 0xFF;
                out_stream[out_start + 1] = (prev_len >> 16) & 0xFF;
                out_stream[out_start + 0] = (prev_len >> 24) & 0xFF;
                out_start += 4;
                for (j = prev_end + 1; j < start_pos; j++)
                {
                    out_stream[out_start] = in_stream[j];
                    out_start++;
                }
                output_nals++;
            }
            prev_start = start_pos;
            prev_end = start_pos + count - 1;
            printf("Detected a start code: start Position is %d and length of start code is %d\n", start_pos, count);
            start_flag = 0;
            count = 0;
            start_pos = -1;
        }
        else if (in_stream[i] != 0 && count == 1)
        {
            start_flag = 0;
            count = 0;
            start_pos = -1;
        }

        // Handle last NAL copy here. This should also handle the case wherein
        // a stream only has one start code
        if ((i == in_len - 1) && output_nals == num_nals - 1)
        {
            int32_t prev_len = (in_len)-(prev_end + 1);
            out_stream[out_start + 3] = prev_len & 0xFF;
            out_stream[out_start + 2] = (prev_len >> 8) & 0xFF;
            out_stream[out_start + 1] = (prev_len >> 16) & 0xFF;
            out_stream[out_start + 0] = (prev_len >> 24) & 0xFF;
            out_start += 4;
            for (j = prev_end + 1; j < (in_len); j++)
            {
                out_stream[out_start] = in_stream[j];
                out_start++;
            }
        }
    }

    print_stream(out_stream, out_start);
}
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int main(void)
{
    printf("Convert an elementary AVC stream to \"box\" format\n");
    uint8_t in_len = 41;
    uint8_t out_len = 44;
    uint8_t *in_stream = malloc(in_len);
    uint8_t *out_stream = malloc(out_len);
    fill_input(in_stream);
    convert_nal_units(in_stream, in_len, out_stream, out_len);
    free(out_stream);
    free(in_stream);
    return 0;
}