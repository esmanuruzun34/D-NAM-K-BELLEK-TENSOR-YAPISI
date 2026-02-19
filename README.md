D-NAM-K-BELLEK-TENSOR-YAPISI

TinyML Dinamik Tensor Kütüphanesi (C Dili)

 Proje Tanımı

Bu proje, RAM kapasitesi sınırlı mikrodenetleyicilerde (Arduino, ESP32
vb.) çalışabilecek şekilde tasarlanmış ilkel (primitive) bir Tensor
veri yapısı  gerçekleştirmesidir.

Standart 32-bit float dizileri gömülü sistemlerde belleği hızla
tüketmektedir. Bu nedenle bu çalışmada, tek bir Tensor yapısı içerisinde
birden fazla veri tipi desteklenerek bellek optimizasyonu sağlanmıştır.

Desteklenen veri tipleri:

-   32-bit Float (FP32)
-   16-bit Float (Basitleştirilmiş FP16)
-   8-bit Integer (Quantized INT8)

------------------------------------------------------------------------

 Proje Amacı

Bu çalışmanın amacı:

-   Gömülü sistemlerde bellek kullanımını azaltmak
-   Tek bir yapı ile farklı sayısal veri tiplerini yönetebilmek
-   Quantization ve Dequantization işlemlerini uygulamak
-   Temel matris çarpımı işlemini gerçekleştirmek
-   Veri tiplerine göre bellek kullanımını analiz etmek

Bu proje tamamen C dili kullanılarak geliştirilmiştir.

------------------------------------------------------------------------

 Desteklenen Veri Tipleri

  Veri Tipi        Eleman Başına Bellek   Açıklama
  ---------------- ---------------------- ---------------------------------
  TENSOR_FLOAT32   4 byte                 Standart IEEE 754 float
  TENSOR_FLOAT16   2 byte                 Basitleştirilmiş half precision
  TENSOR_INT8      1 byte                 8-bit quantized integer

------------------------------------------------------------------------

 Tensor Yapısı

Tensor yapısı aşağıdaki bileşenlerden oluşur:

-   Satır ve sütun bilgisi
-   Veri tipi bilgisi
-   Quantization parametreleri (scale, zero_point)
-   Union kullanılarak tanımlanmış dinamik veri alanı

Bu tasarım sayesinde farklı veri tipleri tek bir yapı altında
yönetilebilmektedir.

------------------------------------------------------------------------

 Gerçekleştirilen Özellikler

 1) Tensor Oluşturma

Seçilen veri tipine göre `malloc()` ile dinamik bellek tahsisi yapılır.

 2) Float32 ↔ Float16 Dönüşümü

IEEE 754 bit düzeyinde değil, sadeleştirilmiş ölçekleme yöntemi
kullanılmıştır.
 
  3) Quantization (Float32 → INT8)

Formül:

    q = (değer / scale) + zero_point

Sonuç INT8 aralığında (-128, 127) sınırlandırılır.

4) Dequantization

Formül:

    değer = (q - zero_point) * scale

Yaklaşık float değer yeniden elde edilir.

 5) Matris Çarpımı

-   Farklı veri tipleri desteklenir
-   Hesaplama sırasında değerler float'a dönüştürülür
-   Sonuç FLOAT32 olarak saklanır

 6) Bellek Kullanım Analizi

Tensor tipine göre toplam bellek kullanımı hesaplanır.

------------------------------------------------------------------------

 Örnek Uygulama

`main()` fonksiyonu içerisinde:

-   FLOAT32 tensor oluşturma
-   FLOAT16 dönüşümü
-   INT8 quantization
-   Dequantization işlemi
-   Matris çarpımı
-   Bellek kullanım karşılaştırması

gerçekleştirilmiştir.

------------------------------------------------------------------------

 Bellek Karşılaştırması (2x2 Tensor)

  Veri Tipi   Toplam Bellek
  ----------- ---------------
  Float32     16 byte
  Float16     8 byte
  INT8        4 byte

Quantization kullanılarak bellek tüketimi %75'e kadar
azaltılabilmektedir.

------------------------------------------------------------------------

Derleme ve Çalıştırma

GCC ile derlemek için:

    gcc tensor.c -o tensor -lm

Çalıştırmak için:

    ./tensor

------------------------------------------------------------------------

 Akademik Amaç

Bu proje:

-   Gömülü sistemler
-   TinyML uygulamaları
-   Bellek optimizasyon teknikleri
-   C dilinde düşük seviyeli veri yapıları

konularında eğitim amaçlı hazırlanmıştır.




