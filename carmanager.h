#ifndef CARMANAGER_H
#define CARMANAGER_H
#include "car.h"
#include <QList>

// Режим відображення каталогу авто (спільний для вікна адміна й користувача)
enum class CatalogMode {
    None,            // без додаткової обробки
    SortPrice,       // сортування злиттям за ціною
    MinMaxMileage,   // лише авто з мінімальним і максимальним пробігом
    GroupClassYear   // групування за класом, всередині — за роком (новіші вище)
};

class CarManager
{
private:
    QList<Car> cars;
    QString filename;
    int nextId = 1;
    QString generateId();
    QList<Car> merge(const QList<Car>& left, const QList<Car>& right, bool isAscending = true);
public:
    CarManager(QString _filename = "cars.json");
    ~CarManager();

    void saveToJson() const;
    void loadFromJson();
    void addCar(const QString& _brand = "Meno", const QString& _model = "Regan", const Category& _category = Category::Econom, const int& _manufacturerYear = 2004, const int& _seats = 4, const double& _rentPrice = 751.0, const double& _mileage = 196.42, const Status& _status = Status::NotAvailable);
    const QList<Car>& getAllCars() const;
    Car* getCarById(const QString& _uid);
    QList<Car> mergeSort(const QList<Car>& list, bool isAscending = true);
    QList<Car> selectMinMaxMileage(const QList<Car>& list) const;
    QList<Car> groupByClassYear(QList<Car> list) const;
    
    void saveToFile(const QString& filePath) const;

};

#endif // CARMANAGER_H
