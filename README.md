# SpaceLazerProject
**Bir lazer pointer iler gök cisimlerinin yerini bulmayı kolaylaştırmak için bir bir  cihaz geliştirmek amaçlanmıştır. Proje kapsamında PIC16F877A, çeşitli  elektronik komponentler, esp32 dev-module, Python tabanlı web serverı kullanılarak gerçekleştirilmiştir.**


Projedek temel olarak mercekli teleskoplardaki kundak ayarından ilham  alınmıştır. Astronomi literatür araştırması yapılmış ve hesaplaması ve  uygulaması daha kolay olacağı için Alt-Az kundağına göre ilerlenmiştir. Yükseklik ve Ufuk Açısı (Altitude & Azimuth kısaca Alt-Az) Koordinatlar Yükseklik, bildiğimiz şekilde yüksekliği ifade eder. Gök cisminin yerden  (ufuktan), dik açısal uzaklığıdır. Ufuk açısı ise, ufukta kuzeyden açısal olarak ne kadar doğuda olduğunu ifade eder.

##Problemin Tanımı ve Çözüm Yöntemleri
Projenin temel amacı, gözlemini yaptığımız gök cismine lazer doğrultmak için pan ve tilt yapabilen bir servo motor kiti kullanmaktı. Ancak, bu süreçte karşılaştığımız bazı zorluklar ve çözüm yöntemleri şunlardır:
Sensör Kütüphanesi Eksikliği: Gözlemimizin doğruluğunu artırmak için yön tayin etmek amacıyla HMC5883L pusula sensörünü kullandım. Ancak, PIC için uygun bir sensör kütüphanesi bulamadığımdan, sensör verisini anlamlı bir şekilde okuyamadım ve transformasyon işlemlerini gerçekleştiremedim.
ESP32 Entegrasyonu: Sensör verisini doğru bir şekilde okuyabilmek ve işleyebilmek için ESP32 kullanarak sistemi wifi ağına bağladım. Bu sayede sensör verisini doğru bir şekilde okuyabildim ve projeyi geliştirebildim.
Astronomik Hesaplamalar: Astronomik hesaplamalar için AstroPy kütüphanesine ihtiyaç duydum. Bu nedenle, ayrı bir Python kodu yazarak bu kütüphaneyi kullandım.
Veri Aktarımı: ESP32 kartına sensör verisini göndermek için Python Flask kütüphanesini kullanarak bir web uygulaması üzerinden HTTP istekleri ile veri alışverişi sağladım.
Servo Motor Hassasiyeti: Servo motorların hassas kontrolü için uzun araştırmalar yaptım. Ancak, kullandığım servo motorların dişli oranlarından dolayı ve PIC16F877A kullanarak istediğim hassasiyette sürmem mümkün olmadı. Projenin başlangıcında hedeflediğim hassasiyet 0.1 dereceydi.

##Yazılım Tasarımı
Yazılım tasarımında, PIC16F877A ile servo motor sürme konusunda deneyimim olmadığı için bir süre araştırma yaptım. 50 Hz RC servoların daha stabil bir şekilde sürülebilmesi için 20 MHz kristal kullanılması gerektiğini öğrendim ve bu yöntemi uygulamak için gerekli malzemeleri sipariş ettim. Ardından, Proteus ortamında simülasyonlar yaparak tasarımı gözden geçirdim.
Gözleme alınacak gezegen belirlendikten sonra, ESP32'ye seri haberleşme ile gezegen indeksi iletiliyor ve ESP32 bu indekse göre HTTP isteği ile çalışan bir Python kodundan hesaplanmış açı değerlerini alıyor. Daha sonra pusula sensörü ile kuzey yönü hesaplanıyor. Bu veriler, gezegenlerin Alt Az açı verisi ve kuzey yönü derecesi olarak PIC'e gönderiliyor. PIC üzerinde gerekli hesaplamalar yapılıyor ve motorların hareketi bu bilgilere göre gerçekleştiriliyor.
sensorAngleCalculate fonksiyonu, sensör verilerini ve gök cisminin açısal pozisyonunu işleyerek, lazerin hareketini kontrol eder. LCD ekran, seçilen gök cisminin adını ve açısal verilerini gösterir. Program, kullanıcının bir gök cismini seçmesine ve ardından seçilen gök cisminin gözlem açısını hesaplamasına olanak tanıyan bir menü yapısı içerir. Kullanıcı menüden bir gök cismini seçtikten sonra, sensör verileri alınır ve sensorAngleCalculate fonksiyonu kullanılarak lazerin konumu ayarlanır.
Kesme servisleri, kullanıcının butonlara basma eylemlerini algılar ve menüde gezinmesine ve seçim yapmasına olanak tanır. Ana döngü içinde, haberleşme için gerekli kod parçaları çalışır ve veri alışverişi sağlanır. Sensör verileri sürekli olarak kontrol edilir ve güncellenir. Modüler fonksiyon yapısı ile kodun daha sade ve anlaşılır olması hedeflenir.

##Tartışma ve Sonuçlar
Servo motorların istenilen hassasiyette açı kontrolü yapılamadı, ancak yaklaşık değerlerle doğru bir sürüş gerçekleştirildi.
Sensör verilerini PIC üzerinden doğru bir şekilde okuyamadım ve bu sorunu çözmek için ESP32 kartını sisteme dahil ettim ve sistemi wifi ağı ile birleştirdim. Alternatif olarak, seri haberleşme ile doğrudan bilgisayar ile iletişim kurabilirdim.
Menü için basit bir fonksiyon yapısı geliştirdim.
PIC16F877A ile proje geliştirmenin, hazır fonksiyonları kullanmadan oldukça zor ve zahmetli olduğuna karar verdim. LCD ve kesme fonksiyonları olmadan projenin yapılma süresinin uzayacağını fark ettim.
Bu proje geliştirme süreci, mikroişlemcilerin temellerini daha iyi anlama ve uygulama şansı sağladı, aynı zamanda mevcut bilgilerimi pekiştirme fırsatı verdi.
