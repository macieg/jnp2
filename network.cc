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

/*
 * Typ sieci.
 */
enum netType
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

/*
 * Komparator leksykograficzny krawędzi.
 */
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
//TODO na co nam te referencje
typedef std::pair<nodes&, vertices&> networkGraph;

/*
 * Reprezentacja sieci.
 */
typedef boost::tuple<nodes, vertices, netType> network;

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
//TODO na co nam ten static
setOfNetworks &getNetworks()
{
	static setOfNetworks res;
	return res;
}

/*
 * Zwraca dostępne do wykorzystania ID. W przypadku świeżo zainicjalizowanej
 * zwraca 0, w przeciwnym przypadku ID następujące bezpośrednio po
 * dotychczasowo największym.
 */
inline ID getFreeID()
{
	if(getNetworks().empty())
		return 0;
	else
		return (--getNetworks().end())->first + 1;
}

/*
 * Zwraca reprezentację pustej sieci wraz z poprawnie nadanym ID.
 */
//TODO na co nam ten static?
std::pair<ID, network> newNetwork(const netType &nType)
{
	static nodes n = nodes();
	static vertices v = vertices();
	return singleNetwork(getFreeID(), network(n, v, nType));
}

/*
 * Sprawdza czy istnieje sieć o podanym ID.
 */
inline bool doesNetworkExist(const ID &idToCheck)
{
	return getNetworks().find(idToCheck) != getNetworks().end();
}

/*
 * W przypadku istniena sieci reprezentowanej poprzez ID zwraca jej wierzchołki.
 */
inline nodes &getNodes(const ID &id)
{
	return boost::get<0>(getNetworks().find(id)->second);
}

/*
 * W przypadku istniena sieci reprezentowanej poprzez ID zwraca jej krawędzie.
 */
inline vertices &getVertices(const ID &id)
{
	return boost::get<1>(getNetworks().find(id)->second);
}
/*
 * W przypadku istnienia sieci reprezentowanej poprzez ID zwraca jej typ.
 */
inline netType &getNetType(const ID &id)
{
	return boost::get<2>((getNetworks().find(id)->second));
}

/*
 * Tworzy nową, pustą, sieć i zwraca jej identyfikator. Sieć pusta, to sieć z pustym zbiorem węzłów.
 * Parametr growing mówi o tym, czy nowa sieć ma być rosnąca (growing != 0) czy nie (growing == 0).
 */
unsigned long network_new(int growing)
{
	///TODO COTO \/
	std::ios_base::Init init;
	netType ntype;
	
	if(growing == 0)
		ntype = NOTGROWINGNET;
	else
		ntype = GROWINGNET;
	
	singleNetwork ret = newNetwork(ntype);
	getNetworks().insert(ret);
	
	unsigned long result = ret.first;
	
	if(debug)
	{
		std::cerr << "network_new(" << growing << ")" << std::endl;
		std::cerr << "network_new: network " << result<< " created" << std::endl;
	}
	
	return result;
}

/*
 * Jeżeli istnieje sieć o identyfikatorze id, to usuwa sieć, a w przeciwnym przypadku nic nie robi.
 */
void network_delete(unsigned long id)
{
	if(debug)
		std::cerr << "network_delete(" << id << ")" << std::endl;
	if(doesNetworkExist(id))
	{
		//TODO tutaj przydałoby się chyba wymazywanie obłożyć asercją, póki co
		//TODO wiemy tylko, że sieć istnieje.
		if(debug)
			std::cerr << "newtwork_delete: network " << id << " deleted" << std::endl;
		getNetworks().erase(id);
	}
	else
		if(debug)
			std::cerr << "network_delete: network doesn't exist" << std::endl;
}

/*
 * Jeżeli istnieje sieć o identyfikatorze id, zwraca liczbę jej węzłów, a w przeciwnym przypadku zwraca 0.
 */
size_t network_nodes_number(unsigned long id)
{
	if(debug)
	{
		std::cerr << "network_nodes_number(" << id << ")" << std::endl;
		std::cerr << "newtork_nodes_number: network " << id << " contains ";
	}
	if(doesNetworkExist(id))
	{
		size_t ret = getNodes(id).size();
		std::cerr << ret << " nodes" << std::endl;
		return ret;
	}
	else
	{
		if(debug)
			std::cerr << " 0 nodes" << std::endl;
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
		std::cerr << "network_links_number(" << id << ")" << std::endl;
		std::cerr << "network_links_number: network " << id << " contains ";
	}
	if(doesNetworkExist(id))
	{
		size_t counter = getVertices(id).size();
		
		if(debug)
			std::cerr << counter << " links" << std::endl;
		return counter;
	}
	else
	{
		if(debug)
			std::cerr << "0 links" << std::endl;
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
		std::cerr << "network_add_node(" << id << ", " << label << ")" << std::endl;
		std::cerr << "newtork_add_node: network " << id;
	}
	if(doesNetworkExist(id))
	{
		//Nie ma jeszcze zaznaczonego wierzchołka, możemy dodawać.
		if(getNodes(id).find(node(label)) == getNodes(id).end())
		{
			if(debug)
			{
				std::cerr << ", node " << label << " added" << std::endl;
				std::cerr << "size = " << getNodes(id).size() << std::endl;
			}
			getNodes(id).insert(node(label));
			//TODO DELETE
			std::cerr << "size = " << getNodes(id).size() << std::endl;
		}
		else
			if(debug)
				std::cerr << ", node " << label << " already exists" << std::endl;
	}
	else
		if(debug)
			std::cerr << " doesn't exist" << std::endl;
}

/*
 * Jeżeli istnieje sieć o identyfikatorze, a w niej węzeł o etykiecie label oraz sieć nie jest rosnąca, 
 * to usuwa węzeł z sieci wraz ze wszystkimi krawędziami wchodzącymi i wychodzącymi, a w przeciwnym przypadku nic nie robi.
 */
void network_remove_node(unsigned long id, const char* label)
{
	if(debug)
		std::cerr << "network_remove_node(" << id << ", " << label << ")" << std::endl;
	if(!doesNetworkExist(id))
	{
		if(debug)
			std::cerr << "network_remove_node: network " << id << " doesn't exist" << std::endl;
		return;
	}
	if(getNetType(id) == GROWINGNET)
	{
		if(debug)
			std::cerr << "newtork_remove_node: network " << id << " is a growing network, failed to remove node " << label << std::endl;
		return;
	}
	
	if(getNodes(id).find(node(label)) == getNodes(id).end())
	{
		if(debug)
			std::cerr << "network_remove_node: network " << id << " doesn't contain node " << label << ", failed to remove node " << label << std::endl;
		return;
	}
	
	//Usuwamy wierzchołek wraz z ewentualnymi krawędziami wychodzącymi z niego.
	if(debug)
		std::cerr << "network_remove_node: network " << id << " node " << label << " removed" << std::endl;
	getNodes(id).erase(getNodes(id).find(node(label)));
	
	//Oraz wszystkie krawędzie do niego wchodzące.
	for(vertices::iterator it = getVertices(id).begin(); it != getVertices(id).end(); it++)
	{
		//TODO DELETE
		std::cerr << "TUTEJ it->first=" << it->first << " , it->second=" << it->second << std::endl;
		if(it->first == node(label) || it->second == node(label))
		{
			if(debug)
				std::cerr << "network_remove_node: network " << id << ", link(" << it->first << ", " << it->second << ") removed" << std::endl;
			getVertices(id).erase(it);
		}
	}
}

/*
 * Jeżeli istnieje sieć o identyfikatorze id, slabel != NULL oraz tlabel != NULL, 
 * i sieć ta nie zawiera jeszcze krawędzi (slabel, tlabel), to dodaje krawędź (slabel, tlabel) do sieci, 
 * a w przeciwnym przypadu nic nie robi. Jeżeli w sieci nie istnieje węzeł o etykiecie któregoś z końców krawędzi,
 * to jest on również dodawany.
 */
void network_add_link(unsigned long id, const char* slabel, const char* tlabel)
{
	if(debug)
		std::cerr << "network_add_link(" << id << ", " << slabel << ", " << tlabel << ")" << std::endl;
	if(!doesNetworkExist(id))
	{
		if(debug)
			std::cerr << "network_add_link: network " << id << " doesn't exist" << std::endl;
		return;
	}
	if(slabel == NULL)
	{
		if(debug)
			std::cerr << "network_add_link: network " << id << ", first node's label is NULL" << std::endl;
		return;
	}
	if(tlabel == NULL)
	{
		if(debug)
			std::cerr << "network_add_link: network " << id << ", second node's label is NULL" << std::endl;
		return;
	}
	
	//Dodajemy węzły jeżeli nie istniały do tej pory.
	network_add_node(id, slabel);
	network_add_node(id, tlabel);
	
	//Dodawana krawędź.
	vertex v = vertex(node(slabel), node(tlabel));
	
	//Węzeł początkowy ma już krawędzie wychodzące - musimy zadbać żeby nie zdublować krawędzi.
	//Sprawdzamy czy krawędź już istnieje jeżeli tak przerywamy.
	if(getVertices(id).find(v) != getVertices(id).end())
	{
		if(debug)
			std::cerr << "newtork_add_link: network " << id << ", link(" << slabel << ", " << tlabel << ") already exists" << std::endl;
		return;
	}
	
	if(debug)
		std::cerr << "newtork_add_link: network " << id << ", link(" << slabel << ", " << tlabel << ") added" << std::endl;
	//Dodajemy krawędź.
	getVertices(id).insert(v);
}


/*
 * Jeżeli istnieje sieć o identyfikatorze id, a w niej krawędź (slabel, tlabel), oraz sieć nie jest rosnąca,
 * to usuwa krawędź z sieci, a w przeciwnym przypadku nic nie robi.
 */
void network_remove_link(unsigned long id, const char* slabel, const char* tlabel)
{
	if(debug)
		std::cerr << "network_remove_link(" << id << ", " << slabel << ", " << tlabel << ")" << std::endl;
	if(!doesNetworkExist(id))
	{
		return;
	}
	if(getNetType(id) == GROWINGNET)
	{
		if(debug)
			std::cerr << "network_remove_link: network " << id << " is a growing network, failed to remove link(" << slabel << ", " << tlabel << ")" << std::endl;
		return;
	}
	
	//Usuwana krawędź.
	vertex v = vertex(node(slabel), node(tlabel));
	
	if(getVertices(id).find(v) != getVertices(id).end())
	{
		if(debug)
			std::cerr << "network_remove_link: network " << id << ", link(" << slabel << ", " << tlabel << ") removed" << std::endl;
		getVertices(id).erase(v);
	}
	else
		if(debug)
			std::cerr << "network_remove_link: network " << id << ", link(" << slabel << ", " << tlabel << ") doesn't exist" << std::endl;
}

/*
 * Jeżeli istnieje sieć o identyfikatorze id i sieć nie jest rosnąca, usuwa wszystkie jej węzły i krawędzie,
 * a w przeciwnym przypadku nic nie robi.
 */
void network_clear(unsigned long id)
{
	if(debug)
	{
		std::cerr << "network_clear(" << id << ")" << std::endl;
		std::cerr << "network_clear: network " << id;
	}
	if(!doesNetworkExist(id))
	{
		if(debug)
			std::cerr << " doesn't exist, failed to clear" << std::endl;
		return;
	}
	if(getNetType(id) == GROWINGNET)
	{
		if(debug)
			std::cerr << " is a growing network, failed to clear" << std::endl;
		return;
	}
	
	//Usuwamy graf reprezentujący węzły z krawędziami wyjściowymi.
	getNodes(id).clear();
	//Usuwamy graf reprezentująćy węzły bez krawędzi wyjściowych.
	getVertices(id).clear();
	
	if(debug)
		std::cerr << "network_clear: network " << id << " cleared" << std::endl;
}

/*
 * Jeżeli istnieje sieć o identyfikatorze id, a w niej węzeł o etykiecie label, to zwraca ilość krawędzi wychodzących z tego węzła, a w przeciwnym przypadku zwraca 0.
 */
size_t network_out_degree(unsigned long id, const char* label)
{
	if(debug)
	{
		std::cerr << "network_out_degree(" << id << ", " << label << ")" << std::endl;
		std::cerr << "network_out_degree: network " << id;
	}
	if(!doesNetworkExist(id))
	{
		if(debug)
			std::cerr << " doesn't exist, out degree = 0" << std::endl;
		return 0;
	}
	
	//Węzeł nie istnieje.
	if(getNodes(id).find(node(label)) == getNodes(id).end())
	{
		if(debug)
			std::cerr << ", label " << label << " doesn't exist" << std::endl;
		return 0;
	}
	
	//Zwracamy szukamy krawędzi wychodzących z wierzchołka.
	size_t ret = 0;
	node v = node(label);
	//TODO NIE DZIAŁA :(((
	vertices::iterator it = getVertices(id).lower_bound(vertex(v, node("")));
	
	//TODO zakładam działanie leniwe koniunkcji
	while(it != getVertices(id).end() && it->first == v)
	{
		//TODO DELETE
		std::cerr << "tst ret=" << ret << " , it->first=" << it->first << " , it->second=" << it->second << std::endl;
		ret++;
		it++;
	}
	
	if(debug)
		std::cerr << ", label " << label << " out degree = " << ret << std::endl;
	return ret;
}

/*
 * Jeżeli istnieje sieć o identyfikatorze id, a w niej węzeł o etykiecie label, to zwraca ilość krawędzi wchodzących do tego węzła, a w przeciwnym przypadku zwraca 0.
 */
size_t network_in_degree(unsigned long id, const char* label)
{
	if(debug)
	{
		std::cerr << "network_in_degree(" << id << ", " << label << ")" << std::endl;
		std::cerr << "network_in_degree: network " << id;
	}
	
	if(!doesNetworkExist(id))
	{
		if(debug)
			std::cerr << " doesn't exist, in degree = 0" << std::endl;
		return 0;
	}
	
	//Węzeł nie istnieje.
	if(getNodes(id).find(node(label)) == getNodes(id).end())
	{
		if(debug)
			std::cerr << ", label " << label << " doesn't exist  " << std::endl;
		return 0;
	}
	
	size_t counter = 0;
	node v = node(label);
	
	for(vertices::iterator it = getVertices(id).begin(); it != getVertices(id).end(); it++)
	{
		if(it->second == v)
			counter++;
	}
	
	if(debug)
		std::cerr << ", label " << label << " in degree = " << counter << std::endl;
	return counter;
}
