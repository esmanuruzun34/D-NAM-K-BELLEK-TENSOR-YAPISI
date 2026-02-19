#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

/* =====================================================
   VERI TIPI TANIMI
===================================================== */

typedef enum {
    TENSOR_FLOAT32,
    TENSOR_FLOAT16,
    TENSOR_INT8
} TensorType;

/* =====================================================
   TENSOR YAPISI
===================================================== */

typedef struct {

    uint16_t rows;
    uint16_t cols;
    TensorType type;

    float scale;        // Quantization için
    int zero_point;

    union {
        float* f32;      // 4 byte
        uint16_t* f16;   // 2 byte
        int8_t* i8;      // 1 byte
    } data;

} Tensor;

/* =====================================================
   TENSOR OLUSTURMA
===================================================== */

Tensor createTensor(uint16_t rows, uint16_t cols, TensorType type) {

    Tensor t;
    t.rows = rows;
    t.cols = cols;
    t.type = type;
    t.scale = 1.0f;
    t.zero_point = 0;

    int total = rows * cols;

    if (type == TENSOR_FLOAT32) {
        t.data.f32 = malloc(total * sizeof(float));
        if (!t.data.f32) { printf("Bellek hatasi!\n"); exit(1); }
    }
    else if (type == TENSOR_FLOAT16) {
        t.data.f16 = malloc(total * sizeof(uint16_t));
        if (!t.data.f16) { printf("Bellek hatasi!\n"); exit(1); }
    }
    else {
        t.data.i8 = malloc(total * sizeof(int8_t));
        if (!t.data.i8) { printf("Bellek hatasi!\n"); exit(1); }
    }

    return t;
}

/* =====================================================
   INDEX
===================================================== */

int index2D(Tensor* t, int i, int j) {
    return i * t->cols + j;
}

/* =====================================================
   FLOAT16 DONUSUM (SIMPLIFIED HALF PRECISION)
   Not: IEEE 754 bit-level implementasyon yerine
   sadeleþtirilmiþ 16-bit temsil kullanýlmýþtýr.
===================================================== */

uint16_t float32_to_float16(float value) {
    return (uint16_t)(value * 1000);  // ölçekli saklama
}

float float16_to_float32(uint16_t value) {
    return (float)value / 1000.0f;
}

/* =====================================================
   QUANTIZATION (F32 -> INT8)
===================================================== */

void quantize(Tensor* src, Tensor* dst, float scale, int zero_point) {

    if (src->type != TENSOR_FLOAT32 || dst->type != TENSOR_INT8) {
        printf("Quantize icin FLOAT32 -> INT8 gerekir!\n");
        return;
    }

    if (scale == 0) {
        printf("Scale 0 olamaz!\n");
        return;
    }

    int total = src->rows * src->cols;

    dst->scale = scale;
    dst->zero_point = zero_point;

    for (int i = 0; i < total; i++) {

        float q = (src->data.f32[i] / scale) + zero_point;

        if (q > 127) q = 127;
        if (q < -128) q = -128;

        dst->data.i8[i] = (int8_t)round(q);
    }
}

/* =====================================================
   DEQUANTIZE (INT8 -> F32)
===================================================== */

float dequantize_value(Tensor* t, int idx) {
    return (t->data.i8[idx] - t->zero_point) * t->scale;
}

/* =====================================================
   MATRIS CARPIMI (Float Hesaplama)
===================================================== */

void matmul(Tensor* A, Tensor* B, Tensor* C) {

    if (A->cols != B->rows) {
        printf("Boyut uyumsuz!\n");
        return;
    }

    if (C->type != TENSOR_FLOAT32) {
        printf("Sonuc FLOAT32 olmali!\n");
        return;
    }

    for (int i = 0; i < A->rows; i++) {
        for (int j = 0; j < B->cols; j++) {

            float sum = 0;

            for (int k = 0; k < A->cols; k++) {

                int idxA = index2D(A, i, k);
                int idxB = index2D(B, k, j);

                float a, b;

                // A deðeri oku
                if (A->type == TENSOR_FLOAT32)
                    a = A->data.f32[idxA];
                else if (A->type == TENSOR_FLOAT16)
                    a = float16_to_float32(A->data.f16[idxA]);
                else
                    a = dequantize_value(A, idxA);

                // B deðeri oku
                if (B->type == TENSOR_FLOAT32)
                    b = B->data.f32[idxB];
                else if (B->type == TENSOR_FLOAT16)
                    b = float16_to_float32(B->data.f16[idxB]);
                else
                    b = dequantize_value(B, idxB);

                sum += a * b;
            }

            C->data.f32[index2D(C,i,j)] = sum;
        }
    }
}

/* =====================================================
   BELLEK ANALIZI
===================================================== */

size_t memoryUsage(Tensor* t) {

    int total = t->rows * t->cols;

    if (t->type == TENSOR_FLOAT32)
        return total * sizeof(float);
    else if (t->type == TENSOR_FLOAT16)
        return total * sizeof(uint16_t);
    else
        return total * sizeof(int8_t);
}

/* =====================================================
   YAZDIRMA
===================================================== */

void printTensor(Tensor* t, const char* msg) {

    printf("\n--- %s ---\n", msg);

    for (int i = 0; i < t->rows; i++) {
        for (int j = 0; j < t->cols; j++) {

            int idx = index2D(t,i,j);
            float val;

            if (t->type == TENSOR_FLOAT32)
                val = t->data.f32[idx];
            else if (t->type == TENSOR_FLOAT16)
                val = float16_to_float32(t->data.f16[idx]);
            else
                val = dequantize_value(t, idx);

            printf("%.2f\t", val);
        }
        printf("\n");
    }
}

/* =====================================================
   MAIN (ORNEK UYGULAMA)
===================================================== */

int main() {

    printf("TinyML Tensor Demo\n");

    Tensor A = createTensor(2,2,TENSOR_FLOAT32);

    A.data.f32[0]=1.5f;
    A.data.f32[1]=2.0f;
    A.data.f32[2]=3.5f;
    A.data.f32[3]=4.0f;

    printTensor(&A,"Float32 Tensor");
    printf("Bellek: %zu byte\n", memoryUsage(&A));

    // FLOAT16 demo
    Tensor H = createTensor(2,2,TENSOR_FLOAT16);
    for(int i=0;i<4;i++)
        H.data.f16[i] = float32_to_float16(A.data.f32[i]);

    printTensor(&H,"Float16 Tensor");
    printf("Bellek: %zu byte\n", memoryUsage(&H));

    // INT8 demo
    Tensor Q = createTensor(2,2,TENSOR_INT8);
    quantize(&A,&Q,0.1f,0);

    printTensor(&Q,"INT8 Quantized Tensor");
    printf("Bellek: %zu byte\n", memoryUsage(&Q));

// DEQUANTIZE DEMO
Tensor D = createTensor(2,2,TENSOR_FLOAT32);

for(int i=0;i<4;i++)
    D.data.f32[i] = dequantize_value(&Q, i);

printTensor(&D,"Dequantized Tensor");
printf("Bellek: %zu byte\n", memoryUsage(&D));



    // Matris çarpýmý
    Tensor C = createTensor(2,2,TENSOR_FLOAT32);
    matmul(&A,&A,&C);
    printTensor(&C,"Matris Carpimi");

    free(A.data.f32);
    free(H.data.f16);
    free(Q.data.i8);
    free(C.data.f32);
free(D.data.f32);

    return 0;
}
