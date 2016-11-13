#include <fstream>
#include <iostream>
#include <string>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <boost/serialization/version.hpp>

//#include <boost/serialization/export.hpp> // pour l'apprentissage des classes dérivés destinés à être sérialisé à partir d'un pointeur (sur leurs classes mères)
/*class Base {
   
};
class Derived_one : public Base {
   
};
class Derived_two : public Base {
   
};
BOOST_CLASS_EXPORT_GUID(Derived_one, "derived_one")
BOOST_CLASS_EXPORT_GUID(Derived_two, "derived_two")*/

#ifdef BOOST_INTRUSIVE_BASIC_SERIALIZATION_EXAMPLE
class Note
{
private:
   friend class boost::serialization::access;

   template<class Archive>
   void serialize(Archive & ar, const unsigned int version) {
      ar & numerateur;
      ar & denominateur;
   }

   int numerateur;
   int denominateur;
public:
   Note() {};
   Note(int n, int d) :
      numerateur(n), denominateur(d) {}
   int getNumerateur() { return numerateur; }
   int getDenominateur() { return denominateur; }
};

class Bulletin
{
   friend class boost::serialization::access;
   template<class Archive>
   void serialize(Archive & ar, const unsigned int version) {
      ar & note1;
      ar & note2;
   }
   Note note1;
   Note note2;
protected:
   Bulletin(const Note & n1_, const Note & n2_) :
      note1(n1_), note2(n2_) {}
public:
   Bulletin() {}
   virtual ~Bulletin() {}
};
#else // BOOST_NON_INTRUSIVE_SERIALIZATION_EXAMPLE

//on rajoute
#include <boost/serialization/split_free.hpp>

#include <boost/serialization/base_object.hpp> // pour sérialiser un objet qui a un héritage

class BaseTest
{
public:
   BaseTest() : m_lpszName("Test") {}
private:
   const char* m_lpszName;
};

class Note
{
public:
   Note() {};
   Note(int n, int d) :
      numerateur(n), denominateur(d) {}
   int getNumerateur() const // CONST EST OBLIGATOIRE
   {
      return numerateur;
   }

   int getDenominateur() const // CONST EST OBLIGATOIRE
   {
      return denominateur;
   }
   
   int getCoef() const // CONST EST OBLIGATOIRE
   {
      return coefficient;
   }

   void setNumerateur(int n)
   {
      numerateur = n;
   }

   void setDenominateur(int n)
   {
      denominateur = n;
   }

   void setCoef(int coef)
   {
      coefficient = coef;
   }

private:
   int numerateur;
   int denominateur;
   int coefficient;

};
//BOOST_CLASS_VERSION(Note, 0)
BOOST_CLASS_VERSION(Note, 1) // nouvelle version

class DevoirSurTable : public Note
{
   friend class boost::serialization::access;
   template<class Archive>
   void serialize(Archive & ar, const unsigned int version)
   {
      ar & boost::serialization::base_object<Note>(*this);
      ar & sujetDevoir;
   }
   std::string sujetDevoir;
public:
   /* Note : le constructeur fournit ses données à la classe mère Note */
   DevoirSurTable() : Note(0, 0), sujetDevoir("") {}
   DevoirSurTable(int note_numerateur, int note_denominateur, std::string sujet) : Note(note_numerateur, note_denominateur), sujetDevoir(sujet) {}
   ~DevoirSurTable() {}
};

class Releve
{
   friend class boost::serialization::access;
   Note * contenu[10];
   template<class Archive>
   void serialize(Archive & ar, const unsigned int version) {
   #ifdef ARRAY_CLASSIC_SERIALIZATION
      for (int i = 0; i < 10; ++i)
         ar & contenu[i];
   #else
      ar & contenu;
   #endif
   }
public:
   Releve() {}
};


   #ifdef PUBLIC_MEMBERS
   namespace boost
   {
      namespace serialization {

         template<class Archive>
         void serialize(Archive & ar, Note & g, const unsigned int version)
         {
            ar & g.numerateur;
            ar & g.denominateur;
         }

      } // namespace serialization
   } // namespace boost
   
   #else

   BOOST_SERIALIZATION_SPLIT_FREE(Note)
   namespace boost
   {
      namespace serialization {
         template<class Archive>
         void save(Archive & ar, const Note & n, unsigned int version)
         {
            ar  & n.getNumerateur();
            ar  & n.getDenominateur();
            ar & n.getCoef();
         }
         template<class Archive>
         void load(Archive & ar, Note & n, unsigned int version)
         {
            int a, b;
            ar  & a;
            n.setNumerateur(a);
            ar  & b;
            n.setDenominateur(b);
            
            if (version > 0)
            {
               int c;
               ar  & c;
               n.setCoef(c);
            }
         }


      } // namespace serialization
   } // namespace boost
   #endif

#endif
int main()
{
   std::ofstream ofs("fichierDeSerialisation");

   // Vous avez vu comme je travaille bien ? :)
   //const Note maNoteDePhysisque(20, 20);
   const DevoirSurTable maNoteDePhysisque(15, 20, "Sciences Physiques");


   {
      boost::archive::text_oarchive oa(ofs);
      oa << maNoteDePhysisque;
   }

   /** Quelque temps plus tard… ***/

   //Note monAncienneNote;
   DevoirSurTable monAncienneNote;

   {
      std::ifstream ifs("fichierDeSerialisation");
      boost::archive::text_iarchive ia(ifs);

      ia >> monAncienneNote;
   }

   std::cout << monAncienneNote.getNumerateur() << "/" << monAncienneNote.getDenominateur() << std::endl;

   return 0;
}
