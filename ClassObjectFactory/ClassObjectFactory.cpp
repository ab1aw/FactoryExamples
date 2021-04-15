#include <string>
#include <map>
#include <iostream>

using namespace std;

// Base interface
class IAnimal
{
public:
    IAnimal() { fprintf(stderr, "IAnimal::IAnimal()\n"); };
    virtual ~IAnimal(void) { fprintf(stderr, "IAnimal::~IAnimal()\n"); };

    virtual int GetNumberOfLegs() const = 0;
    virtual void Speak() = 0;
    virtual void Free() = 0;
    
    //string name;
};


// Create function pointer
typedef IAnimal* (__stdcall *CreateAnimalFn)(void);


// Factory for creating instances of IAnimal
class AnimalFactory
{
private:
    AnimalFactory();
    AnimalFactory(const AnimalFactory &) { }
    AnimalFactory &operator=(const AnimalFactory &) { return *this; }

    IAnimal *CreateAnimal(const string &animalName);
    void Register(const string &animalName, CreateAnimalFn pfnCreate);

    typedef map<string, CreateAnimalFn> FactoryMap;
    FactoryMap      m_FactoryMap;

    typedef map<string, IAnimal*> AnimalCollection;
    AnimalCollection      m_AnimalCollection;

public:
    ~AnimalFactory() {  fprintf(stderr, "~AnimalFactory()\n"); m_FactoryMap.clear(); }

    static AnimalFactory *Get() 
    {
        fprintf(stderr, "AnimalFactory::Get()\n");
        static AnimalFactory instance;
		fprintf(stderr, "\tAnimalFactory::Get()\n");
		return &instance; 
    }
    
    IAnimal *GetAnimal(const string &animalName);
};


// IAnimal implementations
class Cat : public IAnimal
{
public:
    int GetNumberOfLegs() const { return 4; }
    void Speak() { cout << "Meow" << endl; }
    void Free() { delete this; }

    static IAnimal * __stdcall Create() { fprintf(stderr, "new Cat()\n"); IAnimal *animal = new Cat(); fprintf(stderr, "\tExit Create(Cat)\n"); return animal; }
};


class Dog : public IAnimal
{
public:
    int GetNumberOfLegs() const { return 4; }
    void Speak() { cout << "Woof" << endl; }
    void Free() { delete this; }

    static IAnimal * __stdcall Create() { fprintf(stderr, "new Dog()\n"); IAnimal *animal = new Dog(); fprintf(stderr, "\tExit Create(Dog)\n"); return animal; }
};


class Spider : public IAnimal // Yeah it isn't really an animal... but you get the idea
{
public:
    int GetNumberOfLegs() const { return 8; }
    void Speak() { }
    void Free() { delete this; }

    static IAnimal * __stdcall Create() { fprintf(stderr, "new Spider()\n"); IAnimal *animal = new Spider(); fprintf(stderr, "\tExit Create(Spider)\n"); return animal; }
};


class Horse : public IAnimal
{
public:
    int GetNumberOfLegs() const { return 4; }
    void Speak() { cout << "A horse is a horse, of course, of course." << endl; }
    void Free() { delete this; }

    static IAnimal * __stdcall Create() { fprintf(stderr, "new Horse()\n"); IAnimal *animal = new Horse(); fprintf(stderr, "\tExit Create(Horse)\n"); return animal; }
};


/* Animal factory constructor. 
Private, called by the singleton accessor on first call.
Register the types of animals here.
*/
AnimalFactory::AnimalFactory()
{
    fprintf(stderr, "AnimalFactory::AnimalFactory()\n");

    Register("Horse", &Horse::Create);
    Register("Cat", &Cat::Create);
    Register("Dog", &Dog::Create);
    Register("Spider", &Spider::Create);

    fprintf(stderr, "\tExit AnimalFactory::AnimalFactory()\n");
}


void AnimalFactory::Register(const string &animalName, CreateAnimalFn pfnCreate)
{
    fprintf(stderr, "AnimalFactory::Register(%s, ...)\n", animalName.c_str());

    m_FactoryMap[animalName] = pfnCreate;

    fprintf(stderr, "\tExit AnimalFactory::Register(%s, ...)\n", animalName.c_str());
}


IAnimal *AnimalFactory::CreateAnimal(const string &animalName)
{
    fprintf(stderr, "AnimalFactory::CreateAnimal(%s)\n", animalName.c_str());

    FactoryMap::iterator it = m_FactoryMap.find(animalName);

    if( it != m_FactoryMap.end() ) {
        fprintf(stderr, "\tFound %s in factory map.\n", animalName.c_str());
        m_AnimalCollection[animalName] = it->second();
		fprintf(stderr, "\tExit AnimalFactory::CreateAnimal(%s)\n", animalName.c_str());
		return m_AnimalCollection[animalName];
    }

    fprintf(stderr, "\tNULL\n");
	fprintf(stderr, "\tExit AnimalFactory::CreateAnimal(%s)\n", animalName.c_str());
    return NULL;
}


IAnimal *AnimalFactory::GetAnimal(const string &animalName)
{
    fprintf(stderr, "AnimalFactory::GetAnimal(%s)\n", animalName.c_str());

    // Check if the factory already created one of these animals.
    // If yes, get that one.
    // Otherwise create it first and then add it to the collection.

    AnimalCollection::iterator it = m_AnimalCollection.find(animalName);
    if( it != m_AnimalCollection.end() ) {
        fprintf(stderr, "\tFound %s in AnimalCollection map.\n", animalName.c_str());
        IAnimal *the_animal = it->second;
		fprintf(stderr, "\tExit AnimalFactory::GetAnimal(%s)\n", animalName.c_str());
		return the_animal;
    }

    fprintf(stderr, "\tNeed to create a %s and add it to the AnimalCollection map.\n", animalName.c_str());
    IAnimal *the_animal = m_AnimalCollection[animalName] = CreateAnimal(animalName);
	fprintf(stderr, "\tExit AnimalFactory::GetAnimal(%s)\n", animalName.c_str());
    return the_animal;
}


int main( int argc, char **argv )
{
    IAnimal *pAnimal = NULL;
    string animalName;

    while( pAnimal == NULL )
    {
        cout << "Type the name of an animal or 'q' to quit: ";
        cin >> animalName;

        if( animalName == "q" )
            break;

        IAnimal *pAnimal = AnimalFactory::Get()->GetAnimal(animalName);
        if( pAnimal )
        {
            cout << "Your animal has " << pAnimal->GetNumberOfLegs() << " legs." << endl;
            cout << "Your animal says: ";
            pAnimal->Speak();
        }
        else
        {
            cout << "That animal doesn't exist in the farm! Choose another!" << endl;
        }

        pAnimal = NULL;
        animalName.clear();
    }
    return 0;
}
