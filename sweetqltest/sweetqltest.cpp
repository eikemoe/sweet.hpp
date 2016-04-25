#include <string>
#include <sstream>
#include <fstream>
#include <boost/regex.hpp>
#include <utility>
#include <iostream>
#include <algorithm>
#include <sweetql.hpp>
#include <unit.hpp>
//#include <conv.hpp>
#include <benchmark.hpp>
#include <mysqlimpl.hpp>

//#include <Wt/Dbo/Dbo>
//#include <Wt/Dbo/backend/Sqlite3>
//#include <Wt/Dbo/Session>

//namespace dbo = Wt::Dbo;

class Person {
public:
	Person() {} // dummy
	Person(const std::string& f, const std::string& l, const std::string& c, 
			const std::string& a, const std::string& a2,
			const std::string& ci, const std::string& s, int z, 
			const std::string& pw, const std::string& pp, const std::string& m, 
			const std::string& w
	) : firstname(f),	 lastname(l),	 company(c),
 		address(a),		 county(a2),	 city(ci),
 		state(s),		 phoneWork(pw),	 phonePrivat(pp),
 		mail(m),		 www(w),		 zip(z) {
	}

	std::string firstname;
	std::string lastname;
	std::string company;
	std::string address;
	std::string county;
	std::string city;
	std::string state;
	std::string phoneWork;
	std::string phonePrivat;
	std::string mail;
	std::string www;
	int64_t zip;

	/*template<class Action>
	void persist(Action& a) {
		dbo::field(a, firstname, "firstname");
		dbo::field(a, lastname, "lastname");
		dbo::field(a, company, "company");
		dbo::field(a, address, "address");
		dbo::field(a, county, "county");
		dbo::field(a, city, "city");
		dbo::field(a, state, "state");
		dbo::field(a, phoneWork, "phonework");
		dbo::field(a, phonePrivat, "phoneprivate");
		dbo::field(a, mail, "mail");
		dbo::field(a, www, "www");
		dbo::field(a, zip, "zip");
	}*/

	static SqlTable<Person>& table() {
		static SqlTable<Person> tab = SqlTable<Person>::sqlTable("Person",
			SqlColumn<Person>("Firstname", 	makeAttr(&Person::firstname, 
												SweetqlFlags::PrimaryKey)),
			SqlColumn<Person>("Lastname", 	makeAttr(&Person::lastname, 
												SweetqlFlags::PrimaryKey)),
			SqlColumn<Person>("Company", 	makeAttr(&Person::company, 
												SweetqlFlags::PrimaryKey)),
			SqlColumn<Person>("Address", 	makeAttr(&Person::address, 
												SweetqlFlags::PrimaryKey)),
			SqlColumn<Person>("County", 	makeAttr(&Person::county)),
			SqlColumn<Person>("City", 		makeAttr(&Person::city)),
			SqlColumn<Person>("State", 		makeAttr(&Person::state)),
			SqlColumn<Person>("PhoneWork", 	makeAttr(&Person::phoneWork)),
			SqlColumn<Person>("PhonePrivat",makeAttr(&Person::phonePrivat)),
			SqlColumn<Person>("Mail", 		makeAttr(&Person::mail)),
			SqlColumn<Person>("Www", 		makeAttr(&Person::www)),
			SqlColumn<Person>("Zip", 		makeAttr(&Person::zip))
		);
		return tab;
	}
};

typedef std::vector<Person> PersonVec;

class Reservation {
public:
	std::string firstname;
	std::string lastname;
	std::string restaurant;
	std::string location;
	int64_t date;
	int64_t tablemem;

	inline Reservation() {}

	static SqlTable<Reservation>& table() {
		static SqlTable<Reservation> tab = 
			SqlTable<Reservation>::sqlTable("Reservation",
				SqlColumn<Reservation>("Firstname", makeAttr(&Reservation::firstname,
					SweetqlFlags::PrimaryKey)),
				SqlColumn<Reservation>("Lastname", makeAttr(&Reservation::lastname,
					SweetqlFlags::PrimaryKey)),
				SqlColumn<Reservation>("Restaurant", makeAttr(&Reservation::restaurant,
					SweetqlFlags::PrimaryKey)),
				SqlColumn<Reservation>("Location", makeAttr(&Reservation::location,
					SweetqlFlags::PrimaryKey)),
				SqlColumn<Reservation>("Date", makeAttr(&Reservation::date,
					SweetqlFlags::PrimaryKey)),
				SqlColumn<Reservation>("Tablem", makeAttr(&Reservation::tablemem,
					SweetqlFlags::PrimaryKey))
			);

		return tab;
	}
};

class ReservationPerson {
public:
	std::string firstname;
	std::string lastname;
	std::string company;
	std::string address;
	std::string county;
	std::string city;
	std::string state;
	std::string phoneWork;
	std::string phonePrivat;
	std::string mail;
	std::string www;
	int64_t zip;
	std::string restaurant;
	std::string location;
	int64_t date;
	int64_t tablemem;
	ReservationPerson() {}

	static SqlTable<ReservationPerson>& table() {
		static SqlTable<ReservationPerson> tab = 
			SqlTable<ReservationPerson>::sqlTable("ReservationPerson",
			SqlColumn<ReservationPerson>("Firstname", 	
					makeAttr(&ReservationPerson::firstname, 
					SweetqlFlags::PrimaryKey)),
			SqlColumn<ReservationPerson>("Lastname", 	
					makeAttr(&ReservationPerson::lastname, 
					SweetqlFlags::PrimaryKey)),
			SqlColumn<ReservationPerson>("Company", 	
					makeAttr(&ReservationPerson::company, 
					SweetqlFlags::PrimaryKey)),
			SqlColumn<ReservationPerson>("Address", 	
					makeAttr(&ReservationPerson::address, 
					SweetqlFlags::PrimaryKey)),
			SqlColumn<ReservationPerson>("County", 		
					makeAttr(&ReservationPerson::county)),
			SqlColumn<ReservationPerson>("City", 		
					makeAttr(&ReservationPerson::city)),
			SqlColumn<ReservationPerson>("State", 		
					makeAttr(&ReservationPerson::state)),
			SqlColumn<ReservationPerson>("PhoneWork", 	
					makeAttr(&ReservationPerson::phoneWork)),
			SqlColumn<ReservationPerson>("PhonePrivat", 
					makeAttr(&ReservationPerson::phonePrivat)),
			SqlColumn<ReservationPerson>("Mail", 		
					makeAttr(&ReservationPerson::mail)),
			SqlColumn<ReservationPerson>("Www", 		
					makeAttr(&ReservationPerson::www)),
			SqlColumn<ReservationPerson>("Zip", 		
					makeAttr(&ReservationPerson::zip)),
			SqlColumn<ReservationPerson>("Restaurant", 	
					makeAttr(&ReservationPerson::restaurant)),
			SqlColumn<ReservationPerson>("Location", 	
					makeAttr(&ReservationPerson::location)),
			SqlColumn<ReservationPerson>("Date", 		
					makeAttr(&ReservationPerson::date)),
			SqlColumn<ReservationPerson>("Tablem", 		
					makeAttr(&ReservationPerson::tablemem))
		);
		return tab;
	}
};

PersonVec parsePersonFile(const std::string& fn) {
	PersonVec ret;
	std::ifstream infile(fn);
	std::vector<std::string> v;
	std::string line;
	boost::regex re("\"[^\"]+\"");
	while(std::getline(infile, line)) {
		boost::sregex_iterator reBe(line.begin(), line.end(), re);
    	boost::sregex_iterator reEn;
		std::transform(reBe, reEn, std::back_inserter(v), [&v]
			(const boost::smatch& it) {
				return it.str().substr(1, it.str().size()-2);
			}
		);
		ret.push_back(Person(v[0], v[1], v[2], v[3], v[4], v[5], v[6],
					stoi(v[7]), v[8], v[9], v[10], v[11]));
		v.clear();
	}

	return ret;
}

UNITTEST(double_create) {
	Sqlite3 dbImpl(":memory:");
	SweetQL<Sqlite3> db(dbImpl);
	db.createTable<Person>();
	db.createTable<Person>();
   // any good way to check existance of a table? SELECT?
}

UNITTEST(drop_non_existing_table) {
	Sqlite3 dbImpl(":memory:");
	SweetQL<Sqlite3> db(dbImpl);
	db.dropTable<Person>();
}

template<typename T, size_t N>
inline size_t arraylen( const T (&)[N]) { return N; }

struct ComplexThing {
	static constexpr size_t MAX_BLOB_LEN = 255;

	ComplexThing()
	{}

	ComplexThing(const std::string& name, const void* b, size_t len)
		: name(name) {
		this->setBlob(b, len);
	}

	std::string name;
	uint8_t blob[MAX_BLOB_LEN];
	size_t blobLen;

private:
	const uint8_t* getBlob() const {
		return this->blob;
	}

	void setBlob(const void* src, size_t len) {
		len = len < MAX_BLOB_LEN ? len : MAX_BLOB_LEN;
		std::memcpy(this->blob, src, len);
		this->blobLen = len;
	}

public:
	bool operator==(const ComplexThing& other) const {
		if(this->name != other.name or this->blobLen != other.blobLen) {
			return false;
		}
		size_t len{this->blobLen < MAX_BLOB_LEN ? this->blobLen : MAX_BLOB_LEN};
		const uint8_t* my_it{this->blob};
		const uint8_t* ot_it{other.blob};
		for(size_t i = 0; i < len; ++i, ++my_it, ++ot_it) {
			if(*my_it != *ot_it) return false;
		}
		return true;
	}

	static SqlTable<ComplexThing>& table() {
		using namespace std::placeholders;
		static auto tab = SqlTable<ComplexThing>::sqlTable("ComplexThings",
				SqlColumn<ComplexThing>("name", makeAttr(&ComplexThing::name, 
						SweetqlFlags::PrimaryKey)),
				SqlColumn<ComplexThing>("blob", makeBlobAttr<ComplexThing>(
						// for the length: we may use a lambda
						[](const ComplexThing& c) -> size_t {
							return c.blobLen;
						},
						// the getter: we may use a bind
						std::bind(&ComplexThing::getBlob,_1),
						// the setter: we may use a member function
						&ComplexThing::setBlob
						))
				);
		return tab;
	}
};

UNITTEST(blob_and_string_attr) {
	Sqlite3 dbImpl(":memory:");
	SweetQL<Sqlite3> db(dbImpl);
	db.createTable<ComplexThing>();

	const char* testname1 = "HELLO WORLD!";
	const uint8_t testblob1[] = {0,1,2,3,4,5,6,7,8,9,10,11};

	ComplexThing ct1{testname1, testblob1, arraylen(testblob1)};

	db.insert(ct1);

	auto sel = db.select<ComplexThing>();
	std::for_each(sel.first, sel.second, [&](const ComplexThing& p) {
			AS_T(p.name == ct1.name);
			AS_T(p == ct1);
	});
}

UNITTEST(access_to_deleted_string) {
	struct IPV6 : public std::array<uint16_t,8> {
		constexpr IPV6() : std::array<uint16_t,8>() {}

		IPV6(const std::string& s) :
			std::array<uint16_t,8>()
		{
			static const size_t MIN_LEN = 5 * this->size() - 1;
			if(s.size() >= MIN_LEN) {
				for(size_t i=0; i<this->size(); ++i){
					this->operator[]( i ) = std::stoi(s.substr(5*i, 4), nullptr, 16);
				}
			} else {
				this->fill(0);
			}
		}

		std::string toString() const {
			std::stringstream ss;
			ss << std::hex;
			for(size_t i=0; i<this->size(); ++i){
				if(i > 0) ss << ':';
				ss << static_cast<int>(this->operator[](i));
			}
			return ss.str();
		}

		static SqlTable<IPV6>& table() {
			static auto tab = SqlTable<IPV6>::sqlTable("TestTable",
					SqlColumn<IPV6>("s", makeStringAttr<IPV6>(
							[](const IPV6& t) -> std::string { return t.toString(); },
							[](IPV6& t, const std::string& s) { t = IPV6(s); },
							SweetqlFlags::PrimaryKey))
					);
			return tab;
		}
	};

	Sqlite3 dbImpl(":memory:");
	SweetQL<Sqlite3> db(dbImpl);
	db.createTable<IPV6>();

	IPV6 t1("1111:2222:3333:4444:5555:6666:7777:8888");
	db.insert(t1);

	{
		IPV6 p(t1);
		AS_T(p.toString() == t1.toString()); // normal copy works
	}

	// getting from db does not work
	auto sel = db.select<IPV6>();
	std::for_each(sel.first, sel.second, [&](const IPV6& p) {
			//std::cout << p.toString() << "==" << t1.toString() << std::endl;
			AS_T(p.toString() == t1.toString());
	});
}

UNITTEST(double_insert) {
	Sqlite3 dbImpl(":memory:");
	SweetQL<Sqlite3> db(dbImpl);
	db.createTable<ComplexThing>();

	const char* testname1 = "HELLO WORLD!";
	const uint8_t testblob1[] = {0};

	ComplexThing ct1{testname1, testblob1, arraylen(testblob1)};

	db.insert(ct1);
	db.insert(ct1, true);

	auto sel = db.select<ComplexThing>();
	AS_T(std::distance(sel.first, sel.second) == 1);
}

UNITTEST(sweetqltest) {
	/*remove("testtable2.db");
	Sqlite3 dbImpl("testtable2.db");
	SweetQL<Sqlite3> db(dbImpl);
	db.createTable<Person>();
	db.createTable<Reservation>();

	sweet::Bench in;
	std::vector<Person> per = parsePersonFile("50000.csv");
	in.stop();
	std::cout<<"Reading the file took "<<in.milli()<<" msec"<<std::endl;

	sweet::Bench insert;
	db.insert<Person>(per.begin(), per.end());
	insert.stop();
	std::cout<<"Writting the persons to the db took "<<insert.milli()
		<<" msec SWEETQL"<<std::endl;

	Reservation r;
	r.firstname = per.front().firstname;
	r.lastname = per.front().lastname;
	r.tablemem = 1338;
	r.location = "China";
	r.date = 3482938498;
	db.insert(r);

	sweet::Bench s;
	Person toDel;
	auto sel(db.select<Person>());
	std::for_each(sel.first, sel.second, [&toDel](const Person& p) {
		//std::cout<<p.firstname<<' ';
		//std::cout<<p.lastname<<' ';
		//std::cout<<p.company<<' ';
		//std::cout<<p.address<<' ';
		//std::cout<<p.county<<' ';
		//std::cout<<p.zip<<' ';
		//std::cout<<p.state<<' ';
		//std::cout<<p.phoneWork<<' ';
		//std::cout<<p.phonePrivat<<' ';
		//std::cout<<p.mail<<' ';
		//std::cout<<p.www;
		//std::cout<<std::endl;
		toDel = p;
	});
	
	s.stop();
	std::cout<<"Iterating the persons of the db took "<<s.milli()
		<<" msec SWEETQL"<<std::endl;

	auto h = db.join<ReservationPerson,Person,Reservation>();
	for(auto it = h.first; it != h.second; ++it) {
		std::cout<<it->firstname<<' ';
		std::cout<<it->lastname<<' ';
		std::cout<<it->company<<' ';
		std::cout<<it->address<<' ';
		std::cout<<it->county<<' ';
		std::cout<<it->zip<<' ';
		std::cout<<it->state<<' ';
		std::cout<<it->phoneWork<<' ';
		std::cout<<it->phonePrivat<<' ';
		std::cout<<it->mail<<' ';
		std::cout<<it->www;
		std::cout<<std::endl;
	}

	remove("testtable2_odb.db");
	dbo::backend::Sqlite3 sqlite3("testtable2_odb.db");
	dbo::Session session;
	session.setConnection(sqlite3);
	session.mapClass<Person>("person");
	session.createTables();

	sweet::Bench in1;
	dbo::Transaction transaction(session);
	for(auto& it : per) {
		auto p(dbo::ptr<Person>(new Person(it)));
		session.add(p);
	}
	transaction.commit();
	in2.stop();
	std::cout<<"Writting the persons to the db took "<<in2.milli()
		<<" msec WT::DBO"<<std::endl;

	sweet::Bench s2;
	dbo::Transaction transaction2(session);

	dbo::ptr<Person> toDel2;
	typedef Wt::Dbo::collection< Wt::Dbo::ptr<Person> > Persons;
	Persons ps = session.find<Person>();
	for(auto& p : ps) {
		//std::cout<<p.firstname<<' ';
		//std::cout<<p.lastname<<' ';
		//std::cout<<p.company<<' ';
		//std::cout<<p.address<<' ';
		//std::cout<<p.county<<' ';
		//std::cout<<p.zip<<' ';
		//std::cout<<p.state<<' ';
		//std::cout<<p.phoneWork<<' ';
		//std::cout<<p.phonePrivat<<' ';
		//std::cout<<p.mail<<' ';
		//std::cout<<p.www;
		//std::cout<<std::endl;
		toDel2 = p;
	};
	
	s2.stop();
	std::cout<<"Iterating the persons of the db took "<<s2.milli()
		<<" msec WT::DBO"<<std::endl;

	*/


	/*sel = db.select<Person>(
		"Firstname=\"Danny\" and Lastname=\"Zeckzer\""
	);

	std::for_each(sel.first, sel.second, [](const Person& p) {
		std::cout<<p.firstname<<' ';
		std::cout<<p.lastname<<' ';
		std::cout<<p.company<<' ';
		std::cout<<p.address<<' ';
		std::cout<<p.county<<' ';
		std::cout<<p.zip<<' ';
		std::cout<<p.state<<' ';
		std::cout<<p.phoneWork<<' ';
		std::cout<<p.phonePrivat<<' ';
		std::cout<<p.mail<<' ';
		std::cout<<p.www<<std::endl;
	});*/
}

UNITTEST(mysqlinit) {
	MySQL m("localhost", "burner", "h5n1x264r", "testdb", 0, "", 0);
}

#ifdef MAIN
int main(int argc, char *argv[])
{
	return sweet::Unit::runTests();	
}
#endif
