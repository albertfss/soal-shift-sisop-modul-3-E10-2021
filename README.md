# **soal-shift-sisop-modul-3-E10-2021**

## **Kelompok E-10**
- Stefanus Albert Kosim (05111940000096)
- Ahmad Luthfi Hanif (05111940000179)
- Albert Filip Silalahi (05111940000116)

# **Soal 1**

- Soal ini dikerjakan oleh 05111940000096 - Stefanus Albert Kosim.
- Keverk diminta untuk membuat server database buku.
- Dilarang menggunakan system() dan execv(). Silahkan dikerjakan sepenuhnya dengan thread dan socket programming.
- Untuk download dan upload silahkan menggunakan file teks dengan ekstensi dan isi bebas (yang ada isinya bukan touch saja dan tidak kosong) dan requirement untuk benar adalah percobaan dengan minimum 5 data.

## **1A**

- ### **Soal**

    Keverk diminta agar client bisa register dan login ke server. Klien yang terhubung bisa banyak namun yang bisa login hanya 1. Data akun berupa id dan password disimpan dalam file `akun.txt` dengan format:
    ```
    id:password
    id2:password2
    ```

- ### **Penyelesaian**

    Client dan Server dibuat seperti yang ada pada modul, namun ditambahi dengan thread agar server dapat menerima multi-connection.
    ```
    while (TRUE) {
        new_fd = accept(server_fd, (struct     sockaddr *)&new_addr, &addrlen);
        if (new_fd >= 0) {
            printf("Accepted a new connection with fd: %d\n", new_fd);
            pthread_create(&tid, NULL, &menu, (void *) &new_fd);
        } else {
            fprintf(stderr, "Accept failed [%s]\n", strerror(errno));
        }
    }
    ```

    Kemudian digunakan join thread untuk menghandle input-output pada client
    ```
    pthread_create(&(tid[0]), NULL, &handleOutput, (void *) &client_fd);
    pthread_create(&(tid[1]), NULL, &handleInput, (void *) &client_fd);

    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);
    ```
    
    Server lalu menghandle input dari client apakah melakukan register atau login. 
    - Jika client melakukan register maka server akan meminta ID dan Password. ID dan Password yang diterima kemudian akan dituliskan (`append`) ke dalam file bernama `akun.txt` sesuai format `id:password` jika tidak ditemukan id yang sama. 

        Pengecekan akun dilakukan dengan membandingkan id yang ada di `akun.txt` (digunakan strtok) dengan id yang diinputkan client.
        ```
        char db[DATA_BUFFER], *tmp;
        while (fscanf(fp, "%s", db) != EOF) {
            tmp = strtok(db, ":");
            if (strcmp(tmp, id) == 0) return true;
        }
        return false;
        ```

    - Jika client melakukan login maka server akan mengecek apakah ada client yang sedang login. Jika ada maka server akan mengirimkan string bahwa server sedang sibuk. Jika tidak ada maka server akan meminta ID dan Password dan akan dilakukan pengecekan akun sesuai data yang tersimpan pada file `akun.txt`. Pengecekan dilakukan dengan membandingkan `id:password` yang diinputkan user dengan yang ada pada file `akun.txt`
        ```
        char db[DATA_BUFFER], input[DATA_BUFFER];
        sprintf(input, "%s:%s", id, password);
        while (fscanf(fp, "%s", db) != EOF) {
            if (strcmp(db, input) == 0) return true;
        }
        return false;
        ```

    Jika client berhasil login, maka server akan meyimpan status bahwa sedang ada client yang login untuk mencegah client lain login.

![1a](https://i.ibb.co/F4FnPp5/Screenshot-from-2021-05-22-21-26-31.png" )

## **1B & 1C**

- ### **Soal**

    Saat server dijalankan, akan dibuat folder `FILES` yang akan berisi file-file sesuai dengan data yang ada pada `files.tsv`. `files.tsv` ini memiliki format
    ```
    <path di server>\t<publisher>\t<tahun publish>\n
    ```
    Lalu client dapat menambah file ke dalam serever dengan command `add` yang akan disimpan ke dalam folder `FILES`. Untuk menambahkan file maka server akan meminta input :
    ```
    Publisher: 
    Tahun Publikasi: 
    Filepath:
    ```
    Filepath ini merupakan path file yang akan dikirim ke server. 
    
    Setelah file diterima oleh server, maka `files.tsv` akan diperbarui sesuai inputan client yang baru ditambahkan.

- ### **Penyelesaian**

    Folder `FILES` dibuat dengan `mkdir()` tepat setelah server dijalankan.
    ```
    mkdir("FILES", 0777);
    ```

    Jika client mengetikkan command `add`, maka dilakukan pengiriman file dari client ke server sesuai dengan `filepath`. 
    
    Sebelum file dikirim, dilakukan pengecekan file pada `files.tsv` terlebih dahulu. Jika ternyata file telah ada maka file tersebut tidak dikirim. Jika file tidak ditemukan, maka dilakukan pengiriman.
    
    Proses pengiriman ini dilakukan dengan mengirim data per byte sampai size file tersebut berhasil terkirim semuanya. Size file diambil dengan menggunakan
    ```
    seek(fp, 0L, SEEK_END);
    int size = ftell(fp);
    ```

    Kemudian server akan menerima per byte sesuai dengan yang dikirim oleh client. Jika file telah selesai diterima oleh server, server kemudian memperbarui `files.tsv` sesuai dengan format yang telah ditentukan.

![1c](https://i.ibb.co/BVc3Ph8/Screenshot-from-2021-05-22-21-45-56.png)

## **1D**

- ### **Soal**
    
    Client diminta agar dapat mengunduh file yang ada pada server. File dicari di dalam `files.tsv`. Hasil unduhannya akan langsung ada pada folder `Client`.

- ### **Penyelesaian**
  
    Server akan meminta `nama file` beserta `extensinya` dari client (`files.ext`). Setelah server menerima nama file tersebut, nama file tersebut kemudian akan dicek dengan nama file yang ada di `files.tsv`. Karena yang tersimpan di `files.tsv` adalah path, maka nama file dicari dengan menggunakan `strrchr`
    ```
    char db[DATA_BUFFER], *tmp;
    while (fscanf(fp, "%s", db) != EOF) {
        strtok(db, "\t")
        char *ret = strrchr(db, '/');
        if (ret) tmp = ret + 1;
        else tmp = ret;
        if (strcmp(tmp, filename) == 0) return true;
    }
    return false;
    ```

    Jika file ada di `files.tsv` maka dilakukan proses pengiriman yang mirip dengan soal **1C**.

Sebelum diunduh:

![1dbefore](https://i.ibb.co/Yb0rzbg/Screenshot-from-2021-05-22-21-49-35.png)

Sesudah diunduh:

![1dsesudah](https://i.ibb.co/FHMP5mN/Screenshot-from-2021-05-22-21-50-49.png)

## **1E**

- ### **Soal**
  
    Client dapat menghapus file yang ada di server, namun yang terhapus hanya data yang ada di `files.tsv`, file pada folder `FILES` hanya di-*rename* menjadi `old-namaFile.extensi` (`old-file1.ext`).

- ### **Penyelesaian**

    Untuk menghapus file, client akan meminta nama file lengkap dengan extensinya kepada client terlebih dahulu. Kemudian akan dilakukan pengecekan file seperti soal **1D**. 
    
    Jika nama file ada di dalam `files.tsv`, maka akan dibuat file tsv sementara (sebut saja `temp.tsv`). File sementara tersebut akan menyalin semua data pada `files.tsv` kecuali data file yang ingin dihapus. Setelah selesai disalin, file `files.tsv` kemudian akan dihapus dan file `temp.tsv` diubah namanya menjadi `files.tsv`.

    File lama lalu direname menjadi `old-filename`
    ```
    FILE *fp = fopen("files.tsv", "a+");
    FILE *tmp_fp = fopen("temp.tsv", "a+");
    while (fgets(db, SIZE_BUFFER, fp)) {
        sscanf(db, "%s\t%s\t%s", currFilePath, publisher, year);
        if (strcmp(getFileName(currFilePath), filename) != 0) { 
            fprintf(tmp_fp, "%s", db);
        }
        memset(db, 0, SIZE_BUFFER);
    }
    fclose(tmp_fp);
    fclose(fp);
    remove("files.tsv");
    rename("temp.tsv", "files.tsv");

    char deletedFileName[DATA_BUFFER];
    sprintf(deletedFileName, "FILES/%s", filename);

    char newFileName[DATA_BUFFER];
    sprintf(newFileName, "FILES/old-%s", filename);

    rename(deletedFileName, newFileName);
    ```

![1e]( https://i.ibb.co/SXrGsr6/Screenshot-from-2021-05-22-21-53-13.png)

## **1F**

- ### **Soal**

    Client dapat melihat semua isi `files.tsv` dengan command `see` dengan output:
    ```
    Nama:
    Publisher:
    Tahun publishing:
    Ekstensi File : 
    Filepath : 

    Nama:
    Publisher:
    Tahun publishing:
    Ekstensi File : 
    Filepath : 
    ```

- ### **Penyelesaian**

    Untuk menampilkan semua isi `files.tsv` dan mengeluarkan output sesuai format, maka `files.tsv` akan dibaca dan akan dipisahkan untuk setiap `\t` yang ditemukan. 
    
    Kemudian untuk nama file beserta ekstensinya, akan dipisah lagi secara tersendiri dengan menggunakan `strrchr` untuk mengantisipasi ada file dengan nama (`nama.nama2.ext`).

![1f](https://i.ibb.co/DVN7vXT/Screenshot-from-2021-05-22-21-53-42.png)

## **1G**

- ### **Soal**

    Client juga dapat melakukan pencarian berdasarkan nama file yang disimpan pada `files.tsv` dengan output yang sama dengan soal **1F**

- ### **Penyelesaian**

    Untuk melakukan pencarian, akan digunakan fungsi yang sama dengan fungsi `see` (**soal 1F**). Perbedaannya adalah terdapat input tambahan, dimana pada command `find` ini server akan meminta string untuk dicari.

    String yang diterima oleh server kemudian akan dicari di dalam `files.tsv` pada bagian nama file yang telah dipisahkan dengan extensinya. Jika ternyata `files.tsv` sudah selesai dibaca hingga akhir dan tidak menemukan file yang mengandung string inputan, maka server mengirim data tidak ditemukan. Jika ditemukan maka server mengirim pesan sesuai dengan **soal 1F**.

![1G](https://i.ibb.co/q5f3rDF/Screenshot-from-2021-05-22-21-54-18.png)

## **1H**

- ### **Soal**

    Dibuat suatu log bernama **running.log** untuk mencatat setiap penambahan dan penghapusan file yang terjadi dengan format berikut:
    ```
    Tambah : File1.ektensi (id:pass)
    Hapus : File2.ektensi (id:pass)
    ```
    Akan dicatat **Tambah** jika client mengirim command `add`, dan dicatat **Hapus** jika client mengirim command `delete`

- ### **Penyelesaian**

    Untuk mencatat setiap penambahan dan penghapusan file, maka file **running.log** akan di append sesuai command yang diinputkan user.
    ```
    FILE *fp = fopen("running.log", "a+");
    cmd = (strcmp(cmd, "add") == 0) ? "Tambah" : "Hapus";
    fprintf(fp, "%s : %s (%s:%s)\n", cmd, filename, auth_user[0], auth_user[1]);
    fclose(fp);
    ```
    auth_user diperoleh saat melakukan login.

![1H](https://i.ibb.co/s57zSy5/Screenshot-from-2021-05-22-21-55-47.png)

# **Soal 2**
- Soal ini dikerjakan oleh Ahmad Luthfi Hanif - 05111940000179.
- Semua matriks berasal dari input ke program.
- Dilarang menggunakan system()

Crypto (kamu) adalah teman Loba. Karena Crypto adalah orang yang sangat menyukai tantangan, dia ingin membantu Loba mengerjakan tugasnya. Detil dari tugas tersebut adalah:

## **2A**

- ### **Soal**

    Membuat program perkalian matrix (4x3 dengan 3x6) dan menampilkan hasilnya. Matriks tersebut nantinya akan berisi angka 1-20 (tidak perlu dibuat filter angka).
    
- ### **Penyelesaian**
    ```
    #define r1 4
    #define c1 3
    #define r2 3
    #define c2 6

    int matrix1[r1][c1];
    int matrix2[r2][c2];
    int matrix_key[r1*c2];
    ```
    r1, c1, r2, dan c2 didefinisikan sebagai ukuran dari matrix1 (r1 x c1) dan matrix2 (r2 x c2). Setelah itu dideklarasikan array 2 dimensi yaitu matrix1 dan matrix2 untuk menyimpan nilai dari matriks yang diinput oleh user. Variabel matrix_key berfungsi untuk menyimpan hasil dari perkalian matrix1 dan matrix2 dan berfungsi untuk memindahkan hasil ke dalam variabel ```res``` yang nantinya akan digunakan untuk shared memory.
    ```
    key_t key = 1945;
    int shmid = shmget(key, sizeof(int) * r1 * c2, IPC_CREAT | 0666);
    int* res = (int*)shmat(shmid, NULL, 0);
    ```
    Membuat shared memory yang berukuran sebesar baris matriks 1 dan kolom matriks 2. Alamat dari shared memory di assign ke variable ```res``` yang nantinya akan menampung hasil dari perkalian matriks.
    ```
    printf("Matriks 1 (4 x 3) :\n");
    for (i = 0; i < r1; i++)
        for (j = 0; j < c1; j++)
            scanf("%d", &matrix1[i][j]);

    printf("Matriks 2 (3 x 6) :\n");
    for (i = 0; i < r2; i++)
        for (j = 0; j < c2; j++)
            scanf("%d", &matrix2[i][j]);
    ```
    Loop yang digunakan agar user dapat menginput matriks 1 dan matriks 2.
    ```
    pthread_t *threadsid;
    threadsid=(pthread_t*)malloc((24)*sizeof(pthread_t));

    int count = 0;
    int *result = NULL;
    ```
    Sebuah thread dengan nama ```threadsid``` dideklarasikan menjadi sebuah array dengan besar baris matriks 1 dan kolom matriks 2 yaitu sebesar 24. Variabel count berfungsi sebagai indeks dari thread. Variabel integer dengan pointer ```result``` dideklarasikan untuk menyimpan nilai sebuah matriks.
    ```
    for(i = 0; i < r1; i++) {
        for(j = 0; j < c2; j++) {
            result=(int *)malloc((24)*sizeof(int));
            result[0]=c1;

            for(k = 0; k < c1; k++) {
                result[k+1]=matrix1[i][k];
            }

            for(k = 0; k < c1; k++) {
                result[k+1+c1]=matrix2[k][j];
            }

            pthread_create(&(threadsid[count++]), NULL, &multiplier,(void*) result);
        }
    }
    ```
    Dilakukan nested loop sebesar baris matriks 1 x kolom matriks 2. Variabel result dijadikan sebagai array yang menyimpan nilai sebesar 24 integer. Didalam loop tersebut dilakukan looping lagi untuk memasukkan nilai matriks 1 dan matriks 2 berdasarkan r2/c1. Kedua nilai matriks tersebut diinput ke dalam array lurus ```result```. Lalu dibuat thread dengan fungsi ```multiplier``` dan argumen berupa ```result```.
    ```
    void *multiplier(void *argv) {
        int *result = (int*)argv;
        int x = 0;
        int y = 0;

        for(x = 1; x <= c1; x++)
            y+=result[x]*result[x+c1];
    
        int *p=(int*)malloc(sizeof(int));
        *p=y;
        pthread_exit(p);
    }
    ```
    Variabel x befungsi sebagai variabel looping dan variabel y berfungsi untuk menyimpan hasil dari perkalian matriks. Lalu dilakukan loop sebesar c1. Nilai y menyimpan hasil dari perkalian array result dengan indeks x dikali array result dengan indeks x+c1. Lalu deklarasikan sebuah array pointer untuk menyimpan nilai dari y. Lalu exit thread.
    ```
    printf("\nHasil :\n");
    for (i = 0; i < r1*c2; i++) {
        void *k;
        pthread_join(threadsid[i], &k);
        int *p=(int *)k;

        printf("%d ",*p);
        if((i + 1) % c2 == 0) printf("\n");

        matrix_key[i] = *p;
    }
    ```
    Lakukan loop sebesar baris matriks 1 dikalikan kolom matriks 2. threadsid dijoin pada loop ini lalu disimpan dalam variabel ```p``` lalu di print nilainya satu per satu. Array matrix_key berfungsi untuk menampung nilai matriks yang telah dikalikan dan shared memory.
    ```
    for (i = 0; i < r1*c2; i++) {
        res[i] = matrix_key[i];
    }
    ```
    Dilakukan looping untuk memindahkan nilai matrix_key ke dalam variabel shared memory ```res```.
    
- ### **Screenshot 2a**
![2a](https://raw.githubusercontent.com/albertfss/soal-shift-sisop-modul-3-E10-2021/main/soal2/Screenshot%20from%202021-05-23%2014-33-26.png)

## **2B**

- ### **Soal**
    Membuat program dengan menggunakan matriks output dari program sebelumnya (program soal2a.c). Kemudian matriks tersebut akan dilakukan perhitungan dengan matrix baru (input user). Perhitungannya adalah setiap cel yang berasal dari matriks A menjadi angka untuk faktorial, lalu cel dari matriks B menjadi batas maksimal faktorialnya matriks.

- ### **Penyelesaian**
    Pada soal ini menggunakan shared memory untuk mendapatkan nilai dari matriks pada soal 2a dan menggunakan thread untuk menghitung faktorial dari matriksnya.
    ```
    typedef long long int ll;

    int *shared_matrix, *matrix_b;
    ll *result;

    typedef struct pair{
        int a;
        int b;
        int index;
    }pair;
    ```
    Dideklarasikan variabel shared_matrix dan matrix_b untuk menampung nilai matriks dari soal 2a dan matriks yang akan diinput sebagai batas. Variabel pointer bertipe long long int ```result``` berfungsi untuk menyimpan nilai akhir dari perhitungan faktorial matriks. struct ```pair``` digunakan untuk passing ke thread. Variabel ```a``` berfungsi untuk menyimpan nilai dari matrix_shared, ```b``` berfungsi untuk menyimpan nilai dari matrix_b dan ```index``` yang berfungsi menyimpan indeks matriks.
    ```
    key_t key = 1945;

    int shmid = shmget(key, sizeof(int)*24, IPC_CREAT | 0666);
    shared_matrix = shmat(shmid, NULL, 0);
    ```
    Berfungsi untuk mengambil shared memory dari soal2a.c yang merupakan matriks A. shared_matriks menampung nilai-nilai dari matriks hasil dari soal2a.c.
    ```
    matrix_b = (int*) malloc(24 * sizeof(int));
    result = (ll*) malloc(24 * sizeof(ll));
    
    printf("Matriks B (4 x 6) :\n");
    for(int i=0;i<24;i++) {
        scanf("%d",&matrix_b[i]);
    }
    ```
    Variabel matrix_b dan result diinisialisasi sebesar 24 integer. Lalu, user menginput isi dari matriks b dengan menggunakan looping.
    ```
    pthread_t threadsid[24];

    for(int i=0;i<24;i++){
        pair *tes = (pair*)malloc(sizeof(*tes));
        tes->a = shared_matrix[i];
        tes->b = matrix_b[i];
        tes->index = i;
        if(pthread_create(&threadsid[i],NULL,factorial,(void *)tes)!=0){
            fprintf(stderr, "error: Cannot create thread # %d\n",i);
        }
    }
    ```
    Deklarasi array thread dengan besar 24 karena terdapat 24 cell dan masing-masing cell terdiri dari 1 thread. Lalu, dilakukan looping dengan batas 24. Didalam looping tersebut dideklarasikan variabel pointer bertipe pair dengan ukuran sebesar variabel tersebut. Secara satu per satu value dari shared_matrix dan matrix_b indeks ke i disimpan ke dalam variabel pair dan nilai looping variabelnya juga disimpan ke dalam variabel pair. Lalu dibuat thread dengan fungsi factorial dan argumen ```tes```.
    ```
    void *factorial(void *y){
        pair *temp = (pair *) y;
        ll temp_result = 1, i;
        if(temp->a == 0 || temp->b == 0){
            result[temp->index] = 0;
        }
        else if(temp->a >= temp->b){
            for (i = temp->a - temp->b + 1; i <= temp->a; i++) temp_result *= i;
            result[temp->index] = temp_result;
        }
        else if(temp->a < temp->b) {
            for (i = 1; i <= temp->a; i++) temp_result *= i;
            result[temp->index] = temp_result;
        }
    }
    ```
    Fungsi ini berfungsi untuk menghitung nilai factorial dari matrix_shared dengan batas matrix_b. Variabel pointer bertipe data pair berfungsi untuk menyimpan nilai dari argumen yang dipassing dari fungsi main. Lalu, deklarasi variabel ```temp_result``` bertipe data long long int untuk menyimpan hasil dari perhitungan faktorial sementara.
    Apabila nilai ```temp->a``` dan ```temp->b``` bernilai 0 maka pada result dengan indeks ke i (nilai indeks yang di passing) akan diisi dengan nilai 0. Apabila ```temp->a``` lebih besar dari sama dengan ```temp->b``` maka akan dilakukan looping dengan inisiasi ```for(i = temp->a - temp->b + 1; i <= temp->a; i++)```. Apabila ```temp->a``` kurang dari ```temp->b``` maka akan dilakukan looping dengan inisiasi ```for(i = 1; i <= temp->a; i++)```. Pada saat looping dilakukan perkalian variabel looping ```i``` dengan variabel yang menyimpan hasil sementara. Lalu hasil disimpan pada variabel ```result```.
    ```
    for (int i = 0; i < 24; ++i){
      if (pthread_join(threadsid[i], NULL))
        {
          fprintf(stderr, "error: Cannot join thread # %d\n", i);
        }
    }

    printf("Hasil :\n");
    for(int i=0;i<24;i++){
        if(i%6==0)
            printf("\n");
        printf("%lld\t",result[i]);
    }
    printf("\n");
    shmdt(shared_matrix);
    ```
    thread dijoin dengan menggunakan looping dengan batas 24. Lalu hasil diprint menggunakan looping dengan batas 24.
    ```
    shmctl(shmid, IPC_RMID, NULL);
    ```
    Digunakan untuk melakukan free shared memory.
    
- ### **Screenshot 2b**
![2b](https://raw.githubusercontent.com/albertfss/soal-shift-sisop-modul-3-E10-2021/main/soal2/Screenshot%20from%202021-05-23%2016-31-39.png)
    
## **2C**

- ### **Soal**
    Crypto juga membuat program (soal2c.c) untuk mengecek 5 proses teratas apa saja yang memakan resource komputernya dengan command “ps aux | sort -nrk 3,3 | head -5”.

- ### **Penyelesaian**
    Pada soal 2c ini digunakan 2 buah pipe dan 3 kali fork untuk mengeksekusi perintah yang diberikan untuk memnunjukkan 5 proses teratas yang memakan resource.
    ```
    int pid;
    int fd1[2];
    int fd2[2];

    if (pipe(fd1) == -1) {
        perror("bad pipe1");
        exit(1);
    }

    if ((pid = fork()) == -1) {
        perror("bad fork1");
        exit(1);
    } 
    else if (pid == 0) {
        dup2(fd1[1], 1);

        close(fd1[0]);
        close(fd1[1]);

        char *argv[] = {"ps", "aux", NULL};
            execv("/bin/ps", argv);
        perror("ps aux execute doesnt work");
        _exit(1);
    }
    ```
    Create pipe 1 dengan nama ```fd1``` lalu gunakan fork untuk mengeksekusi perintah ```ps aux```. Pada bagian ini, program menunjukkan list proses yang sedang berjalan dengan status dan penggunaan sumber dayanya.
    ```
    if (pipe(fd2) == -1) {
        perror("bad pipe2");
        exit(1);
    }

    if ((pid = fork()) == -1) {
        perror("bad fork2");
        exit(1);
    } 
    else if (pid == 0) {
        dup2(fd1[0], 0);
        dup2(fd2[1], 1);

        close(fd1[0]);
        close(fd1[1]);
        close(fd2[0]);
        close(fd2[1]);
 
        char *argv[] = {"sort", "-nrk", "3.3", NULL};
            execv("/usr/bin/sort", argv);
        perror("sort execute doesnt work");
        _exit(1);
    }
    
    close(fd1[0]);
    close(fd1[1]);
    ```
    Create pipe 2 dengan menggunakan fd2 lalu gunakan fork untuk mengeksekusi perintah ```sort -nrk 3,3```. Pada bagian ini, penampilan data pada list akan diurutkan oleh program.
    ```
    if ((pid = fork()) == -1) {
        perror("bad fork3");
        exit(1);
    } 
    else if (pid == 0) {
        dup2(fd2[0], 0);

        close(fd2[0]);
        close(fd2[1]);

        char *argv[] = {"head", "-5", NULL};
            execv("/usr/bin/head", argv);

        perror("head execute doesnt work");
        _exit(1);
    }
    ```
    Gunakan fork lagi untuk mengeksekusi perintah ```head -5```. Pada bagian ini, program akan menampilkan 5 data teratas yang memakan sumber daya dengan urutan dari yang terbesar hingga yang terkecil.

- ### **Screenshot 2c**
![2c](https://raw.githubusercontent.com/albertfss/soal-shift-sisop-modul-3-E10-2021/main/soal2/Screenshot%20from%202021-05-23%2019-25-16.png)

## **Kendala**
- Saat masa praktikum hanya dapat mengerjakan bagian a karena bertabrakan dengan jadwal ETS.
- Untuk bagian c masih belum sepenuhnya memahami cara kerja pipe sehingga terdapat kendala.

# Soal 3
