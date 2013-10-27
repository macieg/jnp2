#ifdef DEBUG
	const bool debug = true;
#else
    const bool debug = false;
#endif

#include <iostream>
#include <algorithm>
#include <boost/tuple/tuple.hpp>
#include <utility>
#include <map>
#include <set>
#include <string>
#include <assert.h>
#include "network.h"

using namespace std;

/* TODO Nie wiem czy nie przekombinowałem z tą reprezentacją,
 * TODO w każdym razie dołączam schemat co i jak.
 * TODO Ze względu na problemy ze wrzucaniem wierzchołków bez
 * TODO wyjściwoych krawędzi na multimapę zdecydowałem się je
 * TODO rozdzielić od wierzchołków z OUTDEG = 0.
 * TODO Trzeba będzie popatrzeć które ze zdań warunkowych podpiąć
 * TODO tylko pod debug-level = 0.
 * TODO Wypisywanie na strumień błedów też zasadniczo jest na słowo honoru
 * TODO chyba trzeba byłoby pododawać asercje, póki co jest pokazany tylko
 * TODO potok wykonywania zdań warunkowych, nic nie wiemy o realnej
 * TODO poprawności wykonania.
 * TODO uwzględnić ten moduł growingnet.h
 * TODO Do wytestowania/wydebugowania, trzeba będzie w szczególności potestować
 * TODO bardziej kompleksowo te zagnieżdżone pętle po iteratorach, przy
 * TODO usuwaniu/sprawdzaniu istnienia wartości w multimapie.
 * TODO Dodać jakąś flagę która będzie uniemożliwiałą wyświetlanie kodu
 * TODO debugującego przy wewnętrznych wywołaniach.
 *
 *                STRUKTURY DANYCH UŻYTE W ROZWIĄZANIU
 *
 *
 *                     unsigned long
 *                     |
 * setOfNetworks ~ map<ID, network>
 *                         |
 * -------------------------
 * |
 * network ~ pair<networkGraph, networkType>
 *                |             |
 * ----------------             enum
 * |
 * networkGraph ~ pair<nodes, vertices>
 *                     |      |
 * ---------------------      |
 * |                          |
 * nodes ~ set<node>          |
 *                            |    |     |              |
 *                                 -------              |
 *                                    |                 |
 *                                    string            |
 *                                    |                 |
 *                               ------                 |
 *                               |                      |
 * vertices ~ set<vertex>                  |
 * |                                                    |
 * ------------------------------------------------------
 *
 * singleNetwork ~ pair<ID, network>
 */


/*
 * Typ sieci.
 */
enum networkType
{
	GROWINGNET,
	NOTGROWINGNET
};

/*
 * Reprezentacja pojedynczego węzła.
 */
typedef std::string node;

/*
 * Reprezentacja pojedynczej krawędzi.
 */
typedef std::pair<node, node> vertex;

struct compareLex
{
	bool operator()(const vertex &v1, const vertex &v2)
	{
		if(v1.first == v2.first)
			return v1.second < v2.second;
		return v1.first < v2.first;
	}
};

/*
 * Reprezentacja ID sieci.
 */
typedef unsigned long ID;

/*
 * Krawędzie grafu.
 */
typedef std::set<vertex, compareLex> vertices;

/*
 * Wierzchołki grafu.
 */
typedef std::set<node> nodes;
/*
 * Reprezentacja grafu reprezentującego sieć.
 */
typedef std::pair<nodes&, vertices&> networkGraph;

/*
 * Reprezentacja sieci.
 */
typedef std::pair<networkGraph, networkType> network;

/*
 * Pojedyncza sieć.
 */
typedef std::pair<ID, network> singleNetwork;

/*
 * Reprezentacja zbioru sieci.
 */
typedef std::map<ID, network> setOfNetworks;

/*
 * Globalna instancja zbioru sieci.
 */
setOfNetworks networks;

/*
 * Zwraca dostępne do wykorzystania ID. W przypadku świeżo zainicjalizowanej
 * zwraca 0, w przeciwnym przypadku ID następujące bezpośrednio po
 * dotychczasowo największym.
 */
inline ID getFreeID()
{
	if(networks.empty())
		return 0;
	else
		return (--networks.end())->first + 1;
}

/*
 * Zwraca reprezentację pustej sieci wraz z poprawnie nadanym ID.
 */
std::pair<ID, network> newNetwork(networkType ntype)
{
	static nodes n = nodes();
	static vertices v = vertices();
	return singleNetwork(getFreeID(), network(networkGraph(n, v), ntype));
}

/*
 * Sprawdza czy istnieje sieć o podanym ID.
 */
inline bool doesNetworkExist(ID idToCheck)
{
	return networks.find(idToCheck) != networks.end();
}

/*
 * W przypadku istnienia sieci reprezentowanej poprzez ID zwraca jej graf.
 */
inline networkGraph &getGraphOfNetwork(ID id)
{
	return (networks.find(id)->second).first;
}

/*
 * W przypadku istnienia sieci reprezentowanej poprzez ID zwraca jej typ.
 */
inline bool getNetworkType(ID id)
{
	return (networks.find(id)->second).second;
}

/*
 * Sprawdza czy w istniejącej sieci istnieje krawędź [v,w].
 */
bool doesNetworkContainVertex(ID id, node v, node w)
{
	cerr << "doesNetworkContainVertex(" << id << ", " << v << ", " << w << ")";
	
	vertices vert = getGraphOfNetwork(id).second;
	return vert.find(vertex(v, w)) != vert.end();
}

/*
 * Tworzy nową, pustą, sieć i zwraca jej identyfikator. Sieć pusta, to sieć z pustym zbiorem węzłów.
 * Parametr growing mówi o tym, czy nowa sieć ma być rosnąca (growing != 0) czy nie (growing == 0).
 */
unsigned long network_new(int growing)
{
	networkType ntype;
	
	if(growing == 0)
		ntype = NOTGROWINGNET;
	else
		ntype = GROWINGNET;
	
	singleNetwork ret = newNetwork(ntype);
	networks.insert(ret);
	
	unsigned long result = ret.first;
	
	if(debug)
	{
		cerr << "network_new(" << growing << ")" << endl;
		cerr << "network_new: network " << result<< " created" << endl;
	}
	
	return result;
}

/*
 * Jeżeli istnieje sieć o identyfikatorze id, to usuwa sieć, a w przeciwnym przypadku nic nie robi.
 */
void network_delete(unsigned long id)
{
	if(debug)
		cerr << "network_delete(" << id << ")" << endl;
	if(doesNetworkExist(id))
	{
		//TODO tutaj przydałoby się chyba wynazywanie obłożyć asercją, póki co
		//TODO wiemy tylko, że sieć istnieje.
		cerr << "newtwork_delete: network " << id << " deleted" << endl;
		networks.erase(id);
	}
	else
		if(debug)
			cerr << "network_delete: network doesn't exist" << endl;
}

/*
 * Jeżeli istnieje sieć o identyfikatorze id, zwraca liczbę jej węzłów, a w przeciwnym przypadku zwraca 0.
 */
size_t network_nodes_number(unsigned long id)
{
	if(debug)
	{
		cerr << "network_nodes_number(" << id << ")" << endl;
		cerr << "newtork_nodes_number: network " << id << " contains ";
	}
	if(doesNetworkExist(id))
	{
		networkGraph graph = getGraphOfNetwork(id);
		size_t ret = graph.first.size();
		cerr << ret << " nodes" << endl;
		return ret;
	}
	else
	{
		if(debug)
			cerr << " 0 nodes" << endl;
		return 0;
	}
}

/*
 * Jeżeli istnieje sieć o identyfikatorze id, zwraca liczbę jej krawędzi, a w przeciwnym przypadku zwraca 0.
 */
size_t network_links_number(unsigned long id)
{
	if(debug)
	{
		cerr << "network_links_number(" << id << ")" << endl;
		cerr << "network_links_number: network " << id << " has ";
	}
	if(doesNetworkExist(id))
	{
		size_t counter = getGraphOfNetwork(id).second.size();
		
		if(debug)
			cerr << counter << " links" << endl;
		return counter;
	}
	else
	{
		if(debug)
			cerr << "0 links" << endl;
		return 0;
	}
}

/*
 * Jeżeli istnieje sieć o identyfikatorze id, label != NULL i sieć ta nie zawiera jeszcze węzła o etykiecie label, 
 * to dodaje węzeł o etykiecie label do sieci, a w przeciwnym przypadku nic nie robi.
 */
void network_add_node(unsigned long id, const char* label)
{
	if(debug)
	{
		cerr << "network_add_node(" << id << ", " << label << ")" << endl;
		cerr << "newtork_add_node: network " << id;
	}
	if(doesNetworkExist(id))
	{
		nodes graph = getGraphOfNetwork(id).first;
		
		//Nie ma jeszcze zaznaczonego wierzchołka, możemy dodawać.
		if(graph.find(node(label)) == graph.end())
		{
			if(debug)
				cerr << ", node " << label << " added" << endl;
			cerr << "size = " << graph.size() << endl;
			graph.insert(node(label));
			cerr << "size = " << graph.size() << endl;
		}
		else
			if(debug)
				cerr << ", node " << label << " already exists" << endl;
	}
	else
		if(debug)
			cerr << " doesn't exist" << endl;
}

/*
 * Jeżeli istnieje sieć o identyfikatorze, a w niej węzeł o etykiecie label oraz sieć nie jest rosnąca, 
 * to usuwa węzeł z sieci wraz ze wszystkimi krawędziami wchodzącymi i wychodzącymi, a w przeciwnym przypadku nic nie robi.
 */
void network_remove_node(unsigned long id, const char* label)
{
	if(debug)
		cerr << "network_remove_node(" << id << ", " << label << ")" << endl;
	if(!doesNetworkExist(id))
	{
		if(debug)
			cerr << "network_remove_node: network " << id << " doesn't exist" << endl;
		return;
	}
	if(getNetworkType(id) == GROWINGNET)
	{
		if(debug)
			cerr << "newtork_remove_node: network " << id << " is a growing network, failed to remove node " << label << endl;
		return;
	}
	
	networkGraph graph = getGraphOfNetwork(id);
	
	//Usuwamy wierzchołek wraz z ewentualnymi krawędziami wychodzącymi z niego.
	if(graph.first.find(node(label)) == graph.first.end())
		if(debug)
		{
			cerr << "network_remove_node: network " << id << " doesn't contain node " << label << ", failed to remove node " << label << endl;
			return;
		}
	
	if(debug)
		cerr << "newtork_remove_node: network " << id << " node " << label << " removed" << endl;
	graph.first.erase(graph.first.find(node(label)));
	
	//Oraz wszystkie krawędzie do niego wchodzące.
	//TODO czy jest wszystko zwracane przez referencję
	vertices vert = graph.second;
	for(vertices::iterator it = vert.begin(); it != vert.begin(); it++)
	{
		if(it->first == node(label) || it->second == node(label))
		{
			if(debug)
				cerr << "network_remove_node: network " << id << ", link(" << it->first << ", " << it->second << ") removed" << endl;
			vert.erase(it);
		}
	}
	
}

/*
 * Jeżeli istnieje sieć o identyfikatorze id, slabel != NULL oraz tlabel != NULL, 
 * i sieć ta nie zawiera jeszcze krawędzi (slabel, tlabel), to dodaje krawędź (slabel, tlabel) do sieci, 
 * a w przeciwnym przypadu nic nie robi. Jeżeli w sieci nie istnieje węzeł o etykiecie któregoś z końców krawędzi,
 * to jest on również dodawany.
 * TODO dopytać o przypadek gdy nie istnieje żaden z węzłów, póki co zakładam, że dodajemy wtedy oba węzły.
 */
void network_add_link(unsigned long id, const char* slabel, const char* tlabel)
{
	if(debug)
		cerr << "network_add_link(" << id << ", " << slabel << ", " << tlabel << ")" << endl;
	if(!doesNetworkExist(id))
	{
		if(debug)
			cerr << "network_add_link: network " << id << " doesn't exist" << endl;
		return;
	}
	if(slabel == NULL)
	{
		if(debug)
			cerr << "network_add_link: network " << id << ", first node's label is NULL" << endl;
		return;
	}
	if(tlabel == NULL)
	{
		if(debug)
			cerr << "network_add_link: network " << id << ", second node's label is NULL" << endl;
		return;
	}
	
	//TODO czy jest zwracana referencja
	vertices vert = getGraphOfNetwork(id).second;
	
	//Dodajemy węzły jeżeli nie istniały do tej pory.
	//TODO wydzielić esencję network_add_node do innej funkcji, aby nie wypluwała przy poniższyk komunikatów na stderr
	network_add_node(id, slabel);
	network_add_node(id, tlabel);
	
	//Węzeł początkowy ma już krawędzie wychodzące - musimy zadbać żeby nie zdublować krawędzi.
	//Sprawdzamy czy krawędź już istnieje jeżeli tak przerywamy.
	if(vert.find(vertex(node(slabel), node(tlabel))) != vert.end())
	{
		if(debug)
			cerr << "newtork_add_link: network " << id << ", link(" << slabel << ", " << tlabel << ") already exists" << endl;
		return;
	}
	
	if(debug)
		cerr << "newtork_add_link: network " << id << ", link(" << slabel << ", " << tlabel << ") added" << endl;
	//Dodajemy krawędź.
	vert.insert(vertex(node(slabel), node(tlabel)));
}


/*
 * Jeżeli istnieje sieć o identyfikatorze id, a w niej krawędź (slabel, tlabel), oraz sieć nie jest rosnąca,
 * to usuwa krawędź z sieci, a w przeciwnym przypadku nic nie robi.
 */
void network_remove_link(unsigned long id, const char* slabel, const char* tlabel)
{
	if(debug)
		cerr << "network_remove_link(" << id << ", " << slabel << ", " << tlabel << ")";
	if(!doesNetworkExist(id))
	{
		return;
	}
	if(getNetworkType(id) == GROWINGNET)
	{
		if(debug)
			cerr << "network_remove_link: network " << id << " is a growing network, failed to remove link(" << slabel << ", " << tlabel << ")" << endl;
		return;
	}
	
	//TODO czy jest przekazywana referencja
	vertices vert = getGraphOfNetwork(id).second;
	vertex v = vertex(node(slabel), node(tlabel));
	
	if(vert.find(v) != vert.end())
	{
		if(debug)
			cerr << "network_remove_link: network " << id << ", link(" << slabel << ", " << tlabel << ") removed" << endl;
		vert.erase(v);
	}
	else
		if(debug)
			cerr << "network_remove_link: network " << id << ", link(" << slabel << ", " << tlabel << ") doesn't exist" << endl;
}

/*
 * Jeżeli istnieje sieć o identyfikatorze id i sieć nie jest rosnąca, usuwa wszystkie jej węzły i krawędzie,
 * a w przeciwnym przypadku nic nie robi.
 */
void network_clear(unsigned long id)
{
	if(debug)
	{
		cerr << "network_clear(" << id << ")" << endl;
		cerr << "network_clear: network " << id;
	}
	if(!doesNetworkExist(id))
	{
		if(debug)
			cerr << " doesn't exist, failed to clear" << endl;
		return;
	}
	if(getNetworkType(id) == GROWINGNET)
	{
		if(debug)
			cerr << " is a growing network, failed to clear" << endl;
		return;
	}
	
	networkGraph graph = getGraphOfNetwork(id);
	//Usuwamy graf reprezentujący węzły z krawędziami wyjściowymi.
	graph.first.clear();
	//Usuwamy graf reprezentująćy węzły bez krawędzi wyjściowych.
	graph.second.clear();
	
	if(debug)
		cerr << "network_clear: network " << id << " cleared" << endl;
}

/*
 * Jeżeli istnieje sieć o identyfikatorze id, a w niej węzeł o etykiecie label, to zwraca ilość krawędzi wychodzących z tego węzła, a w przeciwnym przypadku zwraca 0.
 */
size_t network_out_degree(unsigned long id, const char* label)
{
	if(debug)
	{
		cerr << "network_out_degree(" << id << ", " << label << ")" << endl;
		cerr << "network_out_degree: network " << id;
	}
	if(!doesNetworkExist(id))
	{
		if(debug)
			cerr << " doesn't exist, out degree = 0" << endl;
		return 0;
	}
	
	networkGraph graph = getGraphOfNetwork(id);
	//Węzeł nie istnieje.
	if(graph.first.find(node(label)) == graph.first.end())
	{
		if(debug)
			cerr << ", label " << label << " doesn't exist" << endl;
		return 0;
	}
	
	//Zwracamy szukamy krawędzi wychodzących z wierzchołka.
	//TODO może da się to ładniej zrealizować.
	size_t ret = 0;
	node v = node(label);
	vertices::iterator it = graph.second.lower_bound(vertex(v, node("")));
	
	while(it->first == v)
	{
		ret++;
		it++;
	}
	if(debug)
		cerr << ", label " << label << " out degree = " << ret << endl;
	return ret;
}

/*
 * Jeżeli istnieje sieć o identyfikatorze id, a w niej węzeł o etykiecie label, to zwraca ilość krawędzi wchodzących do tego węzła, a w przeciwnym przypadku zwraca 0.
 */
size_t network_in_degree(unsigned long id, const char* label)
{
	if(debug)
	{
		cerr << "network_in_degree(" << id << ", " << label << ")" << endl;
		cerr << "network_in_degree: network " << id;
	}
	
	if(!doesNetworkExist(id))
	{
		if(debug)
			cerr << " doesn't exist, in degree = 0" << endl;
		return 0;
	}
	
	networkGraph graph = getGraphOfNetwork(id);
	//Węzeł nie istnieje.
	if(graph.first.find(node(label)) == graph.first.end())
	{
		if(debug)
			cerr << ", label " << label << " doesn't exist" << endl;
		return 0;
	}
	
	size_t counter = 0;
	node v = node(label);
	
	for(vertices::iterator it = graph.second.begin(); it != graph.second.end(); it++)
	{
		if(it->second == v)
			counter++;
	}
	
	if(debug)
		cerr << ", label " << label << " in degree = " << counter << endl;
	return counter;
}

