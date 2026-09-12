#ifndef ACO_HPP
#define ACO_HPP

#include <vector>

#include "Graph.hpp"
#include "Ant.hpp"


class ACO
{
	private:
		const Graph& graph;

		std::vector<std::vector<double>> feromonio;

		int quantidadeFormigas;
		int quantidadeIteracoes;


		double alpha;	//peso distância
		double beta;	//peso feromônio

		double sigma;	//evaporação
		double Q;		//constante de depósito
		double feromonioInicial; 


	public:
		ACO(
			const Graph& graph,
			int quantidadeFormigas,
			int quantidadeIteracoes,
			double alpha,
			double beta,
			double sigma,
			double Q,
			double feromonioInicial
		);

		void executar();

	private:

		double calcularProbabilidade(
			const Ant& ant,
			int destino
		);

		double calcularHeuristica(
			int origem,
			int destino
		) const;

		int escolherProximoVertice(
			const Ant& ant
		);

		void evaporarFeromonio();

		void atualizarFeromonio(
			const std::vector<Ant>& formigas
		);

};


#endif