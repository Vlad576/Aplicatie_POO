#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <ostream>
#include <string>
using namespace std;

class EroareFisier : public exception {
    string mesaj;
public:
    EroareFisier(const string& fname) ;
    const char* what() const noexcept override {
        return mesaj.c_str();
    }
};
EroareFisier::EroareFisier(const string& fname) : mesaj("Fisierul " + fname + " nu a putut fi deschis!") {}


class EroareID : public exception {
    string mesaj;
public:
    EroareID(int id) : mesaj("Nu exista element cu ID-ul " + to_string(id)) {}
    const char* what() const noexcept override {
        return mesaj.c_str();
    }
};

class EroareRezervare : public exception {
    string mesaj;
public:
    EroareRezervare() : mesaj("Rezervarea nu poate fi efectuata!") {}
    const char* what() const noexcept override {
        return mesaj.c_str();
    }
};

class Data {
    int zi, luna, an;
public:
    Data(int zi, int luna, int an);

    bool operator==(const Data& other) const;
    bool operator<(const Data& other) const;
    bool operator>(const Data& other) const;
    bool operator<=(const Data& other) const;
    bool operator>=(const Data& other) const;
    friend Data operator+(const Data& d1, int zile);
    friend istream& operator>>(istream& in, Data& d);
    friend ostream& operator<<(ostream& out, const Data& d);

    static Data from_ZZ_LL_AAAA(const string& data_str);
};
Data::Data(int zi, int luna, int an)
    : zi(zi), luna(luna), an(an) {}
bool Data::operator==(const Data& other) const {
    return an == other.an &&
           luna == other.luna &&
           zi == other.zi;
}
bool Data::operator<(const Data& other) const {
    if (an != other.an) return an < other.an;
    if (luna != other.luna) return luna < other.luna;
    return zi < other.zi;
}
bool Data::operator>(const Data& other) const {
    return other < *this;
}
bool Data::operator<=(const Data& other) const {
    return !(*this > other);
}
bool Data::operator>=(const Data& other) const {
    return !(*this < other);
}
Data operator+(const Data& d1, int zile) {
    Data r = d1;

    static const int zile_luna[] =
    {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    r.zi += zile;

    while (true) {
        int max_zile = zile_luna[r.luna];
        if (r.luna == 2) { // an bisect
            bool bisect = (r.an % 4 == 0 && r.an % 100 != 0) || (r.an % 400 == 0);
            if (bisect) max_zile = 29;
        }
        if (r.zi <= max_zile) break;

        r.zi -= max_zile;
        r.luna++;
        if (r.luna > 12) {
            r.luna = 1;
            r.an++;
        }
    }
    return r;
}
istream& operator>>(istream& in, Data& d) {
    string s;
    cout << "Introduceti data (ZZ-LL-AAAA): ";
    in >> s;
    d = Data::from_ZZ_LL_AAAA(s);
    return in;
}
ostream& operator<<(ostream& out, const Data& d) {
    out << (d.zi < 10 ? "0" : "") << d.zi << "-"
        << (d.luna < 10 ? "0" : "") << d.luna << "-"
        << d.an;
    return out;
}
Data Data::from_ZZ_LL_AAAA(const string& data_str) {
    int zi = stoi(data_str.substr(0, 2));
    int luna = stoi(data_str.substr(3, 2));
    int an = stoi(data_str.substr(6, 4));
    return Data(zi, luna, an);
}

class Spatiu {
protected:
    static int id_generator;
    int id;
    int capacitate;
    int marime;
    bool ocupat;
public:
    Spatiu(int , int);
    virtual ~Spatiu();
    int get_id() const { return id; }
    void set_ocupat(bool val) { ocupat = val; }
    bool este_liber() const { return !ocupat; }
    Spatiu(const Spatiu& other)
        : id(++id_generator), capacitate(other.capacitate),
          marime(other.marime), ocupat(other.ocupat) {}
    Spatiu& operator=(const Spatiu& other) {
        if (this != &other) {
            capacitate = other.capacitate;
            marime = other.marime;
            ocupat = other.ocupat;
            // nu copiem id-ul, generam unul nou
            id = ++id_generator;
        }
        return *this;
    }
};
Spatiu::Spatiu(int cap, int marime) : capacitate(cap), marime(marime), ocupat(false) {
    id = ++id_generator;
}
Spatiu::~Spatiu() {}
int Spatiu::id_generator = 0;

class Camera : public Spatiu {
protected:
    bool are_baie;
public:
    Camera(int, int, bool);
    virtual ~Camera();
    // virtual int get_pret() const = 0;
    virtual void afisare(ostream& out) const = 0;

    int get_capacitate() const { return capacitate; }
    int get_marime() const { return marime; }

    friend ostream& operator<<(ostream& out, const Camera& c) {
        c.afisare(out);   // polimorfism: va apela afisarea clasei derivate
        return out;
    }

    Camera(const Camera& other)
        : Spatiu(other), are_baie(other.are_baie) {}

    Camera& operator=(const Camera& other) {
        if (this != &other) {
            Spatiu::operator=(other);
            are_baie = other.are_baie;
        }
        return *this;
    }


};
Camera::Camera(int cap, int marime, bool baie=false) : Spatiu(cap, marime), are_baie(baie) {}
Camera::~Camera() {
    cout<<"Camera cu ID "<<id<<" a fost distrusa."<<endl;
}

class CameraBalcon : public Camera {
    int marime_balcon;
public:
    CameraBalcon(int, int, bool, int);
    virtual ~CameraBalcon();
    void afisare(ostream& out) const override {
        out << "ID Camera: " << get_id() << "\n";
        out << "Capacitate: " << get_capacitate() << "\n";
        out << "Marime: " << get_marime() << "\n";
        out << "Are baie: " << (are_baie ? "Da" : "Nu") << "\n";
        out << "Marime balcon: " << marime_balcon << "\n";
    }
    CameraBalcon(const CameraBalcon& other)
        : Camera(other), marime_balcon(other.marime_balcon) {}
    CameraBalcon& operator=(const CameraBalcon& other) {
        if (this != &other) {
            Camera::operator=(other);
            marime_balcon = other.marime_balcon;
        }
        return *this;
    }
};
CameraBalcon::CameraBalcon(int cap, int marime, bool baie, int marime_balcon)
    : Camera(cap, marime, baie), marime_balcon(marime_balcon) {}
CameraBalcon::~CameraBalcon() {}

class CameraFamilie : public Camera {
    int numar_paturi_extra;
    bool are_bucatarie;
public:
    CameraFamilie(int cap, int marime, bool baie, int paturi_extra, bool bucatarie)
        : Camera(cap, marime, baie), numar_paturi_extra(paturi_extra), are_bucatarie(bucatarie) {}
    virtual ~CameraFamilie() {}

    void afisare(ostream& out) const override {
        out << "ID Camera: " << get_id() << "\n";
        out << "Capacitate: " << get_capacitate() << "\n";
        out << "Marime: " << get_marime() << "\n";
        out << "Are baie: " << (are_baie ? "Da" : "Nu") << "\n";
        out << "Numar paturi extra: " << numar_paturi_extra << "\n";
        out << "Are bucatarie: " << (are_bucatarie ? "Da" : "Nu") << "\n";
    }

    CameraFamilie(const CameraFamilie& other)
        : Camera(other), numar_paturi_extra(other.numar_paturi_extra), are_bucatarie(other.are_bucatarie) {}

    CameraFamilie& operator=(const CameraFamilie& other) {
        if (this != &other) {
            Camera::operator=(other);
            numar_paturi_extra = other.numar_paturi_extra;
            are_bucatarie = other.are_bucatarie;
        }
        return *this;
    }
};

class CameraVIP : public Camera {
    bool are_jacuzzi;
    bool serviciu_camera;
public:
    CameraVIP(int cap, int marime, bool baie, bool jacuzzi, bool serviciu)
        : Camera(cap, marime, baie), are_jacuzzi(jacuzzi), serviciu_camera(serviciu) {}
    virtual ~CameraVIP() {}
    void afisare(ostream& out) const override {
        out << "ID Camera: " << get_id() << "\n";
        out << "Capacitate: " << get_capacitate() << "\n";
        out << "Marime: " << get_marime() << "\n";
        out << "Are baie: " << (are_baie ? "Da" : "Nu") << "\n";
        out << "Are jacuzzi: " << (are_jacuzzi ? "Da" : "Nu") << "\n";
        out << "Serviciu in camera: " << (serviciu_camera ? "Da" : "Nu") << "\n";
    }
    CameraVIP(const CameraVIP& other)
        : Camera(other), are_jacuzzi(other.are_jacuzzi), serviciu_camera(other.serviciu_camera) {}

    CameraVIP& operator=(const CameraVIP& other) {
        if (this != &other) {
            Camera::operator=(other);
            are_jacuzzi = other.are_jacuzzi;
            serviciu_camera = other.serviciu_camera;
        }
        return *this;
    }
};

class Birou : public Spatiu {
protected:
    bool are_proiector;
public:
    Birou(int, int, bool);
    virtual ~Birou();

    virtual void afisare(ostream& out) const {
        out << "ID Birou: " << get_id() << "\n";
        out << "Capacitate: " << capacitate << "\n";
        out << "Marime: " << marime << "\n";
        out << "Are proiector: " << (are_proiector ? "Da" : "Nu") << "\n";
    }
    int get_capacitate() const { return capacitate; }
    int get_marime() const { return marime; }

    friend ostream& operator<<(ostream& out, const Birou& b) {
        b.afisare(out);
        return out;
    }
    Birou(const Birou& other) : Spatiu(other), are_proiector(other.are_proiector) {}

    Birou& operator=(const Birou& other) {
        if (this != &other) {
            Spatiu::operator=(other);
            are_proiector = other.are_proiector;
        }
        return *this;
    }
};
Birou::Birou(int cap, int marime, bool proiector=false) : Spatiu(cap, marime), are_proiector(proiector) {}
Birou::~Birou() {
    cout<<"Biroul cu ID "<<id<<" a fost distrus."<<endl;
}

class BirouConferinte : public Birou {
    int numar_table;
public:
    BirouConferinte(int, int, int , bool);
    virtual ~BirouConferinte();
    void afisare(ostream& out) const override {
        out << "ID Birou: " << get_id() << "\n";
        out << "Capacitate: " << get_capacitate() << "\n";
        out << "Marime: " << get_marime() << "\n";
        out << "Are proiector: " << (are_proiector ? "Da" : "Nu") << "\n";
        out << "Numar table: " << numar_table << "\n";
    }
    BirouConferinte(const BirouConferinte& other)
        : Birou(other), numar_table(other.numar_table) {}

    BirouConferinte& operator=(const BirouConferinte& other) {
        if (this != &other) {
            Birou::operator=(other);
            numar_table = other.numar_table;
        }
        return *this;
    }

};
BirouConferinte::BirouConferinte(int cap, int marime, int numar_table, bool proiector=false)
    : Birou(cap, marime, proiector), numar_table(numar_table) {}
BirouConferinte::~BirouConferinte() {}

class Laborator : public Birou {
    int numar_calculatoare;
public:
    Laborator(int, int, int , bool);
    virtual ~Laborator();

    void afisare(ostream& out) const override {
        out << "ID Birou: " << get_id() << "\n";
        out << "Capacitate: " << get_capacitate() << "\n";
        out << "Marime: " << get_marime() << "\n";
        out << "Are proiector: " << (are_proiector ? "Da" : "Nu") << "\n";
        out << "Numar calculatoare: " << numar_calculatoare << "\n";
    }
    Laborator(const Laborator& other)
        : Birou(other), numar_calculatoare(other.numar_calculatoare) {}

    Laborator& operator=(const Laborator& other) {
        if (this != &other) {
            Birou::operator=(other);
            numar_calculatoare = other.numar_calculatoare;
        }
        return *this;
    }
};
Laborator::Laborator(int cap, int marime, int numar_calculatoare, bool proiector=false)
    : Birou(cap, marime, proiector), numar_calculatoare(numar_calculatoare) {}
Laborator::~Laborator() {}

class Client {
    static int id_generator;
    int id;
    string nume;
    int varsta;
    int numar_rezervari;
public:
    Client(string, int);
    virtual ~Client();

    int get_id() const { return id; }
    friend ostream& operator<<(ostream& out, const Client& c) {
        out << "ID Client: " << c.id << "\n";
        out << "Nume: " << c.nume << "\n";
        out << "Varsta: " << c.varsta << "\n";
        out << "Numar rezervari: " << c.numar_rezervari << "\n";
        return out;
    }
};
Client::Client(string nume, int varsta) : nume(nume), varsta(varsta), numar_rezervari(0) {
    id = ++id_generator;
}
Client::~Client() {}
int Client::id_generator = 0;

template<typename T>
class Rezervare {
    static int id_generator;
    int id;
    Client* client;
    int numar_persoane;
    Data data_inceput;
    Data data_sfarsit;
    T* spatiu;

public:
    Rezervare();
    Rezervare(Client*, int, Data, Data, T*);
    Rezervare(const Rezervare&);
    virtual ~Rezervare();

    Rezervare& operator=(const Rezervare&);

    friend ostream& operator<<(ostream& out, const Rezervare<T>& r) {
        out << "ID Rezervare: " << r.id << "\n";
        out << "Client: " << *(r.client) << "\n";
        out << "Spatiu rezervat: " << *(r.spatiu) << "\n";
        out << "Data inceput: " << r.data_inceput << "\n";
        out << "Data sfarsit: " << r.data_sfarsit << "\n";
        out << "Numar persoane: " << r.numar_persoane << "\n";
        out << "---------------------------\n";
        return out;
    }

};
template<typename T>
Rezervare<T>::Rezervare() : client(0), numar_persoane(0), data_inceput(0,0,0), data_sfarsit(0,0,0) {
    id = ++id_generator;
}
template<typename T>
Rezervare<T>::Rezervare(Client* c, int nr_pers, Data d_inceput, Data d_sfarsit, T* spatii)
    : client(c), numar_persoane(nr_pers), data_inceput(d_inceput), data_sfarsit(d_sfarsit), spatiu(spatii) {
    id = ++id_generator;
}
template<typename T>
Rezervare<T>::Rezervare(const Rezervare& other)
    : client(other.client), numar_persoane(other.numar_persoane), data_inceput(other.data_inceput),
      data_sfarsit(other.data_sfarsit), spatiu(other.spatiu) {
    id = ++id_generator;
}
template<typename T>
Rezervare<T>::~Rezervare() {}
template<typename T>
Rezervare<T>& Rezervare<T>::operator=(const Rezervare& other) {
    if (this != &other) {
        client = other.client;
        numar_persoane = other.numar_persoane;
        data_inceput = other.data_inceput;
        data_sfarsit = other.data_sfarsit;
        spatiu = other.spatiu;
    }
    return *this;
}
template<typename T>
int Rezervare<T>::id_generator = 0;

template<typename T>
T* gaseste_id(vector<T*> vec, int id) {
    for (auto el : vec) {
        if (el->get_id() == id) {
            return el;
        }
    }
    return nullptr;
}

class Meniu {
    static Meniu* instanta;
    Meniu() {}
public:
    static Meniu* getInstance() {
        if (!instanta)
            instanta = new Meniu();
        return instanta;
    }
    void citesteCamereDinFisier(const string& filename, vector<Camera*>& camere) {
        try {
            ifstream fin(filename);
            if (!fin) throw EroareFisier(filename);
            int m;
            fin >> m;
            for (int i = 0; i < m; ++i) {
                int tip, cap, marime, baie;
                fin >> tip >> cap >> marime >> baie;
                if (tip == 1) { // CameraBalcon
                    int marime_balcon; fin >> marime_balcon;
                    camere.push_back(new CameraBalcon(cap, marime, baie!=0, marime_balcon));
                } else if (tip == 2) { // CameraFamilie
                    int paturi_extra, bucatarie; fin >> paturi_extra >> bucatarie;
                    camere.push_back(new CameraFamilie(cap, marime, baie!=0, paturi_extra, bucatarie!=0));
                } else if (tip == 3) { // CameraVIP
                    int jacuzzi, serviciu; fin >> jacuzzi >> serviciu;
                    camere.push_back(new CameraVIP(cap, marime, baie!=0, jacuzzi!=0, serviciu!=0));
                }
            }
            cout << "Camerele au fost citite cu succes!\n";
        }
        catch (const EroareFisier& e) {
            cout << "Eroare fisier: " << e.what() << "\n";
        }


    }

    void citesteBirouriDinFisier(const string& filename, vector<Birou*>& birouri) {
        ifstream fin(filename);
        if (!fin) {
            cerr << "Nu s-a putut deschide fisierul " << filename << "\n";
            return;
        }
        int n; fin >> n;
        for (int i = 0; i < n; ++i) {
            int tip; fin >> tip;
            if (tip == 1) { // Birou
                int cap, marime, proiector; fin >> cap >> marime >> proiector;
                birouri.push_back(new Birou(cap, marime, proiector));
            } else if (tip == 2) { // Laborator
                int cap, marime, num_calc, proiector; fin >> cap >> marime >> num_calc >> proiector;
                birouri.push_back(new Laborator(cap, marime, num_calc, proiector));
            } else if (tip == 3) { // BirouConferinte
                int cap, marime, num_table, proiector; fin >> cap >> marime >> num_table >> proiector;
                birouri.push_back(new BirouConferinte(cap, marime, num_table, proiector));
            }
        }
        cout << "Birourile au fost citite cu succes!\n";
    }

    void citesteClientiDinFisier(const string& filename, vector<Client*>& clienti) {
        ifstream fin(filename);
        if (!fin) {
            cerr << "Nu s-a putut deschide fisierul " << filename << "\n";
            return;
        }

        int n;
        fin >> n;
        for (int i = 0; i < n; ++i) {
            string nume, prenume;
            int varsta;
            fin >> nume >> prenume >> varsta;
            clienti.push_back(new Client(nume + " " + prenume, varsta));
        }

        cout << "Clientii au fost cititi cu succes!\n";
    }

    void creeazaRezervareCamera(vector<Client*>& clienti,
                                vector<Camera*>& camere,
                                vector<Rezervare<Camera>*>& rezervari_camere, istream& in = cin)
    {
        try {
            if (clienti.empty() || camere.empty())
                throw EroareRezervare();

            int client_id;
            cout << "Introduceti id-ul clientului: ";
            in >> client_id;

            Client* c = gaseste_id(clienti, client_id);
            if (!c) throw EroareID(client_id);

            int camera_id;
            cout << "Introduceti id-ul camerei: ";
            in >> camera_id;

            Camera* cam = gaseste_id(camere, camera_id);
            if (!cam) throw EroareID(camera_id);
            if (!cam->este_liber())
                throw EroareRezervare();

            cam->set_ocupat(true);

            int nr_persoane;
            cout << "Introduceti numarul de persoane: ";
            in >> nr_persoane;

            Data d_start(0,0,0), d_end(0,0,0);
            cout<<"Introduceti data de start si data de final:\n";
            in >> d_start >> d_end;

            Rezervare<Camera>* r = new Rezervare<Camera>(c, nr_persoane, d_start, d_end, cam);
            rezervari_camere.push_back(r);

            cout << "Rezervarea camerei a fost adaugata cu succes!\n";
        }
        catch (const EroareID& e) {
            cout << "Eroare ID:" <<e.what() << "\n";
        }
        catch (const EroareRezervare& e) {
            cout << "Eroare rezervare: "<< e.what() << "\n";
        }
        catch (const exception& e) {
            cout << "Eroare necunoscuta: " << e.what() << "\n";
            throw;
        }

    }

    void creeazaRezervareBirou(vector<Client*>& clienti,
                               vector<Birou*>& birouri,
                               vector<Rezervare<Birou>*>& rezervari_birouri, istream& in = cin)
    {
        if (clienti.empty() || birouri.empty()) {
            cout << "Nu exista clienti sau birouri disponibile.\n";
            return;
        }

        int client_id;
        cout << "Introduceti id-ul clientului: ";
        in >> client_id;

        Client* c = gaseste_id(clienti, client_id);
        if (!c) {
            cout << "Clientul cu acest ID nu exista!\n";
            return;
        }

        int birou_id;
        cout << "Introduceti id-ul biroului: ";
        in >> birou_id;

        Birou* b = gaseste_id(birouri, birou_id);
        if (!b) {
            cout << "Biroul cu acest ID nu exista!\n";
            return;
        }
        b->set_ocupat(true);
        int nr_persoane;
        cout << "Introduceti numarul de persoane: ";
        in >> nr_persoane;

        Data d_start(0,0,0), d_end(0,0,0);
        in >> d_start >> d_end;

        Rezervare<Birou>* r = new Rezervare<Birou>(c, nr_persoane, d_start, d_end, b);
        rezervari_birouri.push_back(r);

        cout << "Rezervarea biroului a fost adaugata cu succes!\n";
    }

    void elibereazaCamera(vector<Camera*>& camere, istream& in = cin) {
        cout << "Introduceti id-ul camerei: ";
        int camera_id; in >> camera_id;
        Camera* cam = gaseste_id(camere, camera_id);
        if (!cam) {
            cout << "Camera cu acest ID nu exista!\n";
            return;
        }
        cam->set_ocupat(false);

        cout << "Camera a fost curatata\n";

        if (dynamic_cast<CameraBalcon*>(cam)) {
            cout << "Balconul a fost curatat.\n";
        } else if (dynamic_cast<CameraFamilie*>(cam)) {
            cout << "Bucataria a fost curatata.\n";
        } else if (dynamic_cast<CameraVIP*>(cam)) {
            cout << "Jacuzzi-ul a fost curatat.\n";
        }

        cout << "Camera cu ID " << camera_id << " a fost eliberata.\n";
    }

    void elibereazaBirou(vector<Birou*>& birouri, istream& in = cin) {
        cout << "Introduceti id-ul biroului: ";
        int birou_id; in >> birou_id;
        Birou* b = gaseste_id(birouri, birou_id);
        if (!b) {
            cout << "Biroul cu acest ID nu exista!\n";
            return;
        }
        b->set_ocupat(false);

        cout << "Biroul a fost curatat.\n";

        if (dynamic_cast<Laborator*>(b)) {
            cout << "Calculatoarele au fost dezinfectate.\n";
        } else if (dynamic_cast<BirouConferinte*>(b)) {
            cout << "Tablele au fost sterse.\n";
        }

        cout << "Biroul cu ID " << birou_id << " a fost eliberat.\n";
    }

    void afiseazaCamereLibere(vector<Camera*>& camere) {
        vector<Camera*> libere;

        for (auto c : camere) {
            if (c->este_liber()) {
                libere.push_back(c);
            }
        }

        sort(libere.begin(), libere.end(), [](Camera* a, Camera* b) {
            return a->get_capacitate() < b->get_capacitate();
        });

        cout << "Camerele libere, sortate dupa capacitate:\n";
        for (auto c : libere) {
            cout << *c << "\n";
        }
    }

    void afiseaza(vector<Client*>& clienti,vector <Camera*>& camere, vector<Birou*>& birouri,
        vector<Rezervare<Camera>*>& rezervari_camere, vector<Rezervare<Birou>*>& rezervari_birouri, istream& in = cin) {
        while (true) {
            cout << "===== MENIU =====\n";
            cout << "1. Citeste camere din fisier\n";
            cout << "2. Citeste birouri din fisier\n";
            cout << "3. Citeste clienti din fisier\n";
            cout << "4. Afiseaza camere\n";
            cout << "5. Afiseaza birouri\n";
            cout << "6. Afiseaza clienti\n";
            cout << "7. Creeaza rezervare camera\n";
            cout << "8. Creeaza rezervare birou\n";
            cout << "9. Afiseaza rezervari camere\n";
            cout << "10. Afiseaza rezervari birouri\n";
            cout << "11. Eliberare camera\n";
            cout << "12. Eliberare birou\n";
            cout << "13. Afiseaza camere libere sortate dupa capacitate\n";
            cout << "0. Iesire\n";
            cout << "Alegeti o optiune: ";
            int opt; in >> opt;
            cout << "Ati ales optiunea: " << opt << "\n";
            if (opt == 0) break;
            switch (opt) {
                case 1:
                    citesteCamereDinFisier("camere.in", camere);
                    break;
                case 2:
                    citesteBirouriDinFisier("birouri.in",
                        birouri);
                    break;
                case 3:
                    citesteClientiDinFisier("clienti.in",
                        clienti);
                    break;
                case 4:
                    for (auto c : camere)
                        cout << "Camera =" << *c << "\n";
                    break;
                case 5:
                    for (auto b : birouri)
                        cout << "Birou =" << *b << "\n";
                    break;
                case 6:
                    for (auto cl : clienti)
                        cout << "Client =" << *cl << "\n";
                    break;
                case 7:
                    creeazaRezervareCamera(clienti, camere,rezervari_camere, in);
                    break;
                case 8:
                    creeazaRezervareBirou(clienti, birouri, rezervari_birouri, in);
                    break;
                case 9:
                    for (auto r : rezervari_camere)
                        cout << "Rezervare Camera =" << *r << "\n";
                    break;
                case 10:
                    for (auto r : rezervari_birouri)
                        cout << "Rezervare Birou =" << *r << "\n";
                    break;
                case 11:
                    elibereazaCamera(camere, in);
                    break;
                case 12:
                    elibereazaBirou(birouri, in);
                    break;
                case 13:
                    afiseazaCamereLibere(camere);
                    break;
                default:
                    cout << "Optiune invalida!\n";
            }
        }
    }
};

Meniu* Meniu::instanta = nullptr;

int main() {
    cout << "Sigur! Iti voi genera un proiect simplu in C++ care implementeaza un sistem de rezervari "
            "pentru camere si birouri folosind conceptele OOP precum clase, mostenire, polimorfism si exceptii.\n";
    // glumita :)

    std::ifstream fin("tastatura.txt");
    if (!fin) {
        std::cerr << "Nu s-a putut deschide fisierul tastatura.txt\n";
        return 1;
    }
    vector<Client *> clienti;
    vector<Camera *> camere;
    vector<Birou *> birouri;
    vector<Rezervare<Camera> *> rezervari_camere;
    vector<Rezervare<Birou> *> rezervari_birouri;
    try {
        Meniu* meniu = Meniu::getInstance();
        meniu->afiseaza(clienti, camere, birouri, rezervari_camere, rezervari_birouri, fin);
    }
    catch(const exception& e) {
        cout << "Eroare critica: " << e.what() << "\n";
    }

    for(auto c : clienti) delete c;
    for(auto c : camere) delete c;
    for(auto b : birouri) delete b;
    for(auto r : rezervari_camere) delete r;
    for(auto r : rezervari_birouri) delete r;

    return 0;
}
