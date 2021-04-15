#include <string>
#include <map>
#include <iostream>

using namespace std;

// Base interface
class IAnimal
{
public:
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

    typedef map<string, CreateAnimalFn> FactoryMap;
    FactoryMap      m_FactoryMap;
public:
    ~AnimalFactory() {  fprintf(stderr, "~AnimalFactory()\n"); m_FactoryMap.clear(); }

    static AnimalFactory *Get() 
    {
		fprintf(stderr, "AnimalFactory::Get()\n");
        static AnimalFactory instance;
        return &instance; 
    }
    
    void Register(const string &animalName, CreateAnimalFn pfnCreate);
    IAnimal *CreateAnimal(const string &animalName);
};

// IAnimal implementations
class Cat : public IAnimal
{
public:
    int GetNumberOfLegs() const { return 4; }
    void Speak() { cout << "Meow" << endl; }
    void Free() { delete this; }

    static IAnimal * __stdcall Create() { fprintf(stderr, "new Cat()\n"); return new Cat(); }
};

class Dog : public IAnimal
{
public:
    int GetNumberOfLegs() const { return 4; }
    void Speak() { cout << "Woof" << endl; }
    void Free() { delete this; }

    static IAnimal * __stdcall Create() { fprintf(stderr, "new Dog()\n"); return new Dog(); }
};

class Spider : public IAnimal // Yeah it isn't really an animal... but you get the idea
{
public:
    int GetNumberOfLegs() const { return 8; }
    void Speak() { }
    void Free() { delete this; }

    static IAnimal * __stdcall Create() { fprintf(stderr, "new Spider()\n"); return new Spider(); }
};

class Horse : public IAnimal
{
public:
    int GetNumberOfLegs() const { return 4; }
    void Speak() { cout << "A horse is a horse, of course, of course." << endl; }
    void Free() { delete this; }

    static IAnimal * __stdcall Create() { fprintf(stderr, "Horse\n"); return new Horse(); }
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
}

void AnimalFactory::Register(const string &animalName, CreateAnimalFn pfnCreate)
{
    fprintf(stderr, "AnimalFactory::Register(%s, ...)\n", animalName.c_str());

    m_FactoryMap[animalName] = pfnCreate;
}

IAnimal *AnimalFactory::CreateAnimal(const string &animalName)
{
    fprintf(stderr, "AnimalFactory::CreateAnimal(%s)\n", animalName.c_str());

    FactoryMap::iterator it = m_FactoryMap.find(animalName);
    if( it != m_FactoryMap.end() ) {
		fprintf(stderr, "\tFound %s in factory map.\n", animalName.c_str());
        return it->second();
	}

    fprintf(stderr, "\tNULL\n");
    return NULL;
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

        IAnimal *pAnimal = AnimalFactory::Get()->CreateAnimal(animalName);
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
        if( pAnimal )
            pAnimal->Free();
        pAnimal = NULL;
        animalName.clear();
    }
    return 0;
}