#include <iostream>
#include <ctime>
#include <fstream>
#include <vector>
#include <chrono>
#include <algorithm>
#include <string>
#include <map>

using namespace std;

string inputFile = "data.txt";
string outputFile = "output.txt";
string timeStampsFile = "timestamps.txt";

ofstream fout(outputFile);

int batchNum = 7;

int dims[7] = { 100, 500, 1000, 5000, 10000, 50000, 100000 };

//ключ - поле name
struct Product {
    string name;
    string country;
    unsigned int volume;
    unsigned int sum;

    Product() {

    }

    Product(string name, string country, unsigned int volume, unsigned int sum)
    {
        this->name = name;
        this->country = country;
        this->volume = volume;
        this->sum = sum;
    }

    friend bool operator== (const Product& a, const Product& b) {
        if (a.name == b.name && a.volume == b.volume
            && a.country == b.country)
            return true;
        return false;
    }

    friend bool operator< (const Product& a, const Product& b) {
        if (a.name < b.name || ((a.name == b.name) && (a.volume < b.volume)) ||
            ((a.name == b.name) && (a.volume == b.volume) && (a.country < b.country)))
            return true;
        return false;
    }

    friend bool operator<= (const Product& a, const Product& b) {
        if (a < b || a == b)
            return true;
        return false;
    }

    friend bool operator> (const Product& a, const Product& b) {
        if (!(a < b) && !(a == b))
            return true;
        return false;
    }

    friend bool operator>= (const Product& a, const Product& b) {
        if (!(a < b))
            return true;
        return false;
    }


    friend ostream& operator<<(ostream& os, const Product& a) {
        os << a.name << ' ' << a.volume << ' ' << a.country << '\n';
        return os;
    }

    string getName() {
        return this->name;
    }
};

//записывает время 
void writeTime(string title, std::chrono::steady_clock::time_point start, std::chrono::steady_clock::time_point end) {
    fout << title << endl;

    fout << chrono::duration_cast<chrono::microseconds>(end - start).count() << " микросекунд\n\n";
}

//функция прямого поиска, возвращает массив найденных индексов
vector<int> straightSearch(vector<Product>& v, string key) {
    vector<int> res;

    for (int i = 0; i < v.size(); ++i)
        if (v[i].getName() == key)
            res.push_back(i);

    return res;
}

//найти нижнюю границу двоичным поиском
int getLowerBound(vector<Product>& v, string key) {
    int mid = 0, left = 0, right = v.size();
    while (1)
    {
        mid = (left + right) / 2;

        if (mid < 0 || mid >= v.size())
            return -1;

        if (key <= v[mid].getName())
            right = mid - 1;
        else if (key > v[mid].getName() && mid + 1 < v.size() && key == v[mid + 1].getName())
            return mid;
        else if (key > v[mid].getName())
            left = mid + 1;

        if (left > right)
            return -1;
    }
}

//найти верхнюю границу двоичным поиском
int getUpperBound(vector<Product>& v, string key) {
    int mid = 0, left = 0, right = v.size();
    while (1)
    {
        mid = (left + right) / 2;

        if (mid < 0 || mid >= v.size())
            return -1;

        if (key < v[mid].getName() && mid - 1 >= 0 && key == v[mid - 1].getName())
            return mid;
        else if (key < v[mid].getName())
            right = mid - 1;
        else if (key >= v[mid].getName())
            left = mid + 1;

        if (left > right)
            return -1;
    }
}

//сам двоичный поиск
vector<Product> binarySearch(vector<Product>& v, string key) {
    int left = getLowerBound(v, key);
    int right = getUpperBound(v, key);

    if (left == -1 || right == -1)
        return vector<Product>();

    auto first = v.cbegin() + left + 1;
    auto last = v.cbegin() + right;

    vector<Product> res(first, last);

    return res;
}

//Объединить два массива L и M в arr
void merge(vector<Product>& arr, int p, int q, int r) {
    //Создать массивы L = A[p..q] и M = A[q+1..r]
    int n1 = q - p + 1;
    int n2 = r - q;

    Product* L = new Product[n1];
    Product* M = new Product[n2];

    for (int i = 0; i < n1; i++)
        L[i] = arr[p + i];
    for (int j = 0; j < n2; j++)
        M[j] = arr[q + 1 + j];

    //Поддержка 3 активных индексов массивов
    int i, j, k;
    i = 0;
    j = 0;
    k = p;

    while (i < n1 && j < n2) {
        if (L[i] <= M[j]) {
            arr[k] = L[i];
            i++;
        }
        else {
            arr[k] = M[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = M[j];
        j++;
        k++;
    }
}

//Сортировка слиянием
void mergeSort(vector<Product>& arr, int l, int r) {
    if (l < r) {
        // m - это точка разделения над подмассивы
        int m = l + (r - l) / 2;

        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);

        //Объединить отсортированные подмассивы
        merge(arr, l, m, r);
    }
}

//функция для генерации случайной строки
string randomString(const int len) {
    static const char alphanum[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    string tmp_s;
    tmp_s.reserve(len);
    for (int i = 0; i < len; ++i) {
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    return tmp_s;
}

void makeTxtFile() {
    srand((unsigned)time(NULL));
    ofstream fout(inputFile);

    for (int i = 0; i < batchNum; ++i) {
        //Вывод числа записей
        fout << dims[i] << "\n";
        for (int j = 0; j < dims[i]; ++j) {
            //Вывод полей по порядку
            fout << randomString(8) << "\n";    //Имя
            fout << randomString(10) << "\n";   //Страна
            fout << rand() + 1 << "\n";         //Объем
            fout << rand() + 1 << "\n";         //Сумма
        }
    }
}

vector<vector<Product>> readTxtFile() {
    ifstream fin(inputFile);

    vector<vector<Product>> result;

    int dim;
    string name;
    string country;
    unsigned int volume;
    unsigned int sum;

    for (int i = 0; i < batchNum; ++i) {
        //Ввод числа записей
        fin >> dim;
        vector<Product> v;
        for (int j = 0; j < dim; ++j) {
            //Ввод полей по порядку
            fin >> name >> country >> volume >> sum;
            Product temp(name, country, volume, sum);
            v.push_back(temp);
        }
        result.push_back(v);
    }

    return result;
}

int main()
{
    setlocale(LC_ALL, "Russian");

    makeTxtFile();

    //массив с объектами
    vector<vector<Product>> initialArray = readTxtFile();

    //массив с ключами, по которым будет проходить поиск
    vector<string> selectedKeys;

    for (int i = 0; i < batchNum; ++i) {
        selectedKeys.push_back(initialArray[i][rand() % initialArray[i].size()].getName());
    }

    std::chrono::steady_clock::time_point start, end;
    Product foundValue;

    fout << "Прямой поиск:\n\n";

    start = std::chrono::steady_clock::now();

    for (int i = 0; i < batchNum; ++i) {
        vector<int> temp = straightSearch(initialArray[i], selectedKeys[i]);
        end = std::chrono::steady_clock::now();
        fout << "Поиск по ключу: " << selectedKeys[i] << endl;
        if (!temp.empty())
            for (int j = 0; j < temp.size(); ++j)
                fout << "Найденный объект: " << initialArray[i][temp[j]];
        writeTime("На прямой поиск с " + to_string(initialArray[i].size()) + " объектами ушло: ", start, end);
        start = end;
    }

    fout << "Сортировка:" << endl << endl;

    start = std::chrono::steady_clock::now();

    for (int i = 0; i < batchNum; ++i) {
        mergeSort(initialArray[i], 0, initialArray[i].size() - 1);
        binarySearch(initialArray[i], selectedKeys[i]);
        end = std::chrono::steady_clock::now();
        writeTime("На сортировку и двоичный поиск " + to_string(initialArray[i].size()) + " значениий ушло: ", start, end);
        start = end;
    }

    fout << "Бинарный поиск в отсортированном массиве:" << endl;

    start = std::chrono::steady_clock::now();

    for (int i = 0; i < batchNum; ++i) {
        vector<Product> temp = binarySearch(initialArray[i], selectedKeys[i]);
        end = std::chrono::steady_clock::now();
        fout << "Поиск по ключу: " << selectedKeys[i] << "\n";
        if (!temp.empty())
            for (int j = 0; j < temp.size(); ++j)
                fout << "Найденный объект: " << temp[j];
        writeTime("На бинарный поиск с " + to_string(initialArray[i].size()) + " объектами ушло: ", start, end);
        start = end;
    }

    vector<multimap<string, Product>> m;

    for (int i = 0; i < batchNum; ++i) {
        multimap<string, Product> temp;
        for (int j = 0; j < initialArray[i].size(); ++j)
            temp.insert({ initialArray[i][j].getName(), initialArray[i][j] });
        m.push_back(temp);
    }

    initialArray.clear();

    fout << "Поиск в ассоциативном массиве:\n\n";

    start = std::chrono::steady_clock::now();

    for (int i = 0; i < batchNum; ++i) {
        auto bounds = m[i].equal_range(selectedKeys[i]);
        end = std::chrono::steady_clock::now();
        fout << "Поиск по ключу: " << selectedKeys[i] << "\n";
        if (bounds.first != m[i].end())
            for (auto it = bounds.first; it != bounds.second; ++it)
                fout << "Найденный объект: " << it->second;
        writeTime("На поиск в ассоциативном массиве с " + to_string(m[i].size()) + " объектами ушло: ", start, end);
        start = end;
    }

    return 0;
}
