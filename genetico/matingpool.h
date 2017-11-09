#include "genome.h"
#include <random>
#include <stdlib.h>     /* abs */


class PlayerGenetic : public Player {
private:
    Genome g;
public:
    PlayerGenetic(Genome g) : g(g) {}
    int nextMove(Game& game) {

        auto moves = game.board().possibleMoves();
        auto bestCol = max_element(moves.begin(), moves.end(),
            [this, game](const int& m1, const int& m2) {
                return g.activate(game.board(), m1) < g.activate(game.board(), m2);
            }
        );
        assert(bestCol != moves.end()); // encontró alguno

        // cerr << "PossibleMoves: " << endl;
        // for (auto i : game.board().possibleMoves())
        // {
        //     cerr << i <<" ";
        // }

        // cerr << endl << "Genetic elige: " << *bestCol << endl;

        return *bestCol;
    }
};

int minimax(const PossibleMove& node, int depth, bool maximizingPlayer) {
	if (depth == 0 || node.isTerminal()) {
		return node.heuristic();
	}

	if (maximizingPlayer) {
		int bestValue = std::numeric_limits<int>::min();
		for (PossibleMove child : node.children()) {
            // in y out son trampas para poder agregar y sacar fichas en el tablero
			child.in();
			int v = minimax(child, depth - 1, false);
			bestValue = std::max(bestValue, v);
			child.out();
		}

		return bestValue;
	}
	else {
		int bestValue = std::numeric_limits<int>::max();
		for (PossibleMove child : node.children()) {
            // in y out son trampas para poder agregar y sacar fichas en el tablero
			child.in();
			int v = minimax(child, depth - 1, true);
			bestValue = std::min(bestValue, v);
			child.out();
		}

		return bestValue;
	}
}

class PlayerMinimax_n : public Player {
private:
	int plays;
public:
	PlayerMinimax_n(int n) : plays(n){}
	int nextMove(Game& game) {
         // TODO traer p del juego

        auto moves = PossibleMove(game, -1).children(); // -1 ya que no se usa ese valor
        int bestResult = -1;
        vector<int> losers;
        int max = 0;
        for (unsigned int i = 0; i < moves.size();++i) {
            // in y out son trampas para poder agregar y sacar fichas en el tablero
            moves.at(i).in();
            auto v = minimax(moves.at(i), plays - 1, false);
            moves.at(i).out();
            if(v == -1) losers.push_back(i);
            if(bestResult<v){
                bestResult = v;
                max = i;
            }

        }

        if(bestMove == 0){

            bool loser = false;
            do{ 
                // binomial_distribution<int> randomMove(moves.size()/2, 0.5);
                // max = randomMove.operator();
                max = rand() % moves.size();
                for (unsigned int i = 0; i < losers.size(); ++i){
                    if(losers.at(i) == max){
                        loser = true;
                        break;
                    } 
                    if(losers.at(i) > max){
                        break;
                    }
                }
            } while (loser);
        }
        
        return moves.at(max).move();
    }
};


class MatingPool {
public:
    MatingPool(int rows, int cols, int c, int pieces, int amountOfSurvivors, unsigned int populationSize, unsigned int games, float pc, float pm, float t, float mr, float pRandomMating, int fitnessFunction, float alpha, unsigned int extinctionRate);
    vector<Genome> getPopulation();
    Genome crossover(Genome& g1, Genome& g2);
    Genome mitosis(Genome& g1);
    void evolvePopulation(unsigned int generations, int spacing);
private:
    int rows;
    int cols;
    int c;
    int pieces;
    int amountOfSurvivors;
    unsigned int populationSize;
    unsigned int numberOfGamesToPlay;
    float pCrossover;
    float pMutate;
    float crossoverThreshold; // probabilidad de que haya un "corte" durante el crossover
    float mutationRadius;
    float pRandomMating;
    vector<Genome> population;
    vector< Player* > lastChampions;
    float averageFitness;
    unsigned int currentGeneration;
    int fitnessFunction;
    float alpha;
    unsigned int extinctionRate;

    void newGeneration();
    float calculateFitness(Genome g);
    vector<unsigned int> survivorIndices();
};

MatingPool::MatingPool(int rows, int cols, int c, int pieces, int amountOfSurvivors, unsigned int populationSize, unsigned int games, float pc, float pm, float t, float mr, float pRandomMating, int fitnessFunction, float alpha, unsigned int extinctionRate) :
    rows(rows),
    cols(cols),
    c(c),
    pieces(pieces),
    amountOfSurvivors(amountOfSurvivors),
    populationSize(populationSize),
    numberOfGamesToPlay(games),
    pCrossover(pc),
    pMutate(pm),
    crossoverThreshold(t),
    mutationRadius(mr),
    pRandomMating(pRandomMating),
    currentGeneration(0),
    fitnessFunction(fitnessFunction),
    alpha(alpha),
    extinctionRate(extinctionRate) {
        assert(crossoverThreshold >= 0 && crossoverThreshold <= 1 &&
               pMutate >= 0 && pMutate <= 1 && mutationRadius >= 0);
        for (unsigned int i = 0; i < populationSize; ++i) {
            population.push_back(Genome(c));
        }
        for (int i = 0; i < amountOfSurvivors; ++i) {
            // Genome genome (4, { -0.211009, 2.2091, 1.19177, 6.09952, 2.55178, -2.45106, 0.561537, 0.307363, 0.7774, 0.506736, 0.750724, 0.914839, -0.234364, 0.0909093, -0.269583, -0.37189, -0.538976, 0.0200729, 0.0, 0.0, 0.0, 0.192907, 1.02838, 0.762831 });
            // Player* p = new PlayerGenetic(genome);
            Player* p = new PlayerMinimax_n(4);
            lastChampions.push_back(p);
        }
    }

vector<Genome> MatingPool::getPopulation() {
    return population;
}

void MatingPool::newGeneration() {
    vector<unsigned int> fittest = survivorIndices();
    assert(population.size() == populationSize);
    int fitSize = fittest.size();
    assert(fitSize == amountOfSurvivors);

    vector<Genome> newPopulation;

    for (unsigned int j = 0; j < populationSize; ++j) {
		unsigned int firstGenomeIndex = rand() % amountOfSurvivors;
		unsigned int secondGenomeIndex = rand() % amountOfSurvivors;
		while (firstGenomeIndex == secondGenomeIndex) {
			secondGenomeIndex = rand() % amountOfSurvivors;
		}
        Genome& firstGenome = population.at(fittest.at(firstGenomeIndex));
        Genome& secondGenome = population.at(fittest.at(secondGenomeIndex));

        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

        default_random_engine crossOrMitosisGenerator(seed);
        uniform_real_distribution<float> crossOrMitosisDistribution(0.0, 1.0);
        default_random_engine randomMatingGenerator(seed);
        uniform_real_distribution<float> randomMatingDistribution(0.0, 1.0);

        float evolP = crossOrMitosisDistribution(crossOrMitosisGenerator);
        float matingP = randomMatingDistribution(randomMatingGenerator);

        if (matingP <= pRandomMating) {
            // Reproducción con un individuo elegido al azar
            unsigned int randomToSelect = rand() % populationSize;
            newPopulation.push_back(crossover(firstGenome, population.at(randomToSelect)));
        } else {
            if (evolP <= pCrossover) {
                // Reproducción entre dos de los individuos más aptos
                newPopulation.push_back(crossover(firstGenome, secondGenome));
            } else {
                // Replicación de uno de los individuos más aptos con posibilidad de mutación
                newPopulation.push_back(mitosis(firstGenome));
            }
        }
    }
    if (currentGeneration % extinctionRate == extinctionRate - 1) {
        // queremos entrenar a los primeros campeones contra algo mejor que random
        for (int i = 0; i < amountOfSurvivors; ++i) {
            *(lastChampions.at(i)) = PlayerGenetic(population.at( fittest.at(i)) );
        }
    }

    population = newPopulation;
}

void MatingPool::evolvePopulation(unsigned int generations, int spacing) {
    for (; currentGeneration < generations; ++currentGeneration) {
        #ifdef SHOWSENSEI
        cerr << "Generacion: " << currentGeneration << " --------------------------" << endl;
        #endif
        if (currentGeneration % extinctionRate == 0 && currentGeneration > 0) {
            vector<unsigned int> fittest = survivorIndices();
            // cerr << "MASS EXTINCTION" << endl;
            for (unsigned int i = 0; i < populationSize; ++i) {
                population.at(i) = Genome(c);
            }
        } else {
            newGeneration();
        }

    }

    #ifdef GENECORRELATION
    for (auto genome : population) {
        for (int i = 0; i < genome.geneWeights.size(); ++i) {
           cerr << genome.geneWeights.at(i);
           if(i != genome.geneWeights.size() -1){
            cerr << ";";
           }
        }
        cerr << endl;
    }
    #endif
    #ifdef OUTPUTBEST
    ofstream outputFile;
    outputFile.open("../experimentacion/sensei.txt");
    displayVector(population.at(0).geneWeights, outputFile); // el individuo de mayor fitness
    outputFile.close();
    #endif
}

float MatingPool::calculateFitness(Genome g) {
    PlayerGenetic player(g);
    // Genome genome(4, { -0.211009, 2.2091, 1.19177, 6.09952, 2.55178, -2.45106, 0.561537, 0.307363, 0.7774, 0.506736, 0.750724, 0.914839, -0.234364, 0.0909093, -0.269583, -0.37189, -0.538976, 0.0200729, 0.192907, 1.02838, 0.762831 });
    // PlayerGenetic sensei(genome);

    int wins = 0;
    int numberOfMovesToWin = 1; //So we never divide by 0
    int numberOfMovesToLose = 0;

    for (unsigned int i = 0; i < numberOfGamesToPlay; ++i) {
        Game game(rows, cols, c, pieces);
        int champion = rand() % lastChampions.size();
        pair<int,int > result = game.playMatch(player, *(lastChampions.at(champion)) );

        wins += result.first;
        if(result.first){
            numberOfMovesToWin += result.second;
        } else {
            numberOfMovesToLose += result.second;
        }
    }




    if (fitnessFunction == 1) {
        return (float) wins / numberOfGamesToPlay;
    } else {

        int value = 0;
        for (auto w: g.geneWeights)
        {
            value = abs(w);
        }


        return ((1 - alpha)* ((float) wins / numberOfGamesToPlay)) - alpha *value;
            // + (alpha * numberOfMovesToLose / numberOfMovesToWin);
            
    }
}

Genome MatingPool::crossover(Genome& g1, Genome& g2) {
    // cerr << "Parent 1: " << endl;
    // displayVector(g1.geneWeights);
    // cerr << "Parent 2: " << endl;
    // displayVector(g2.geneWeights);

    vector<float> newWeights;
    Genome* activeGenome = &g1;

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    default_random_engine cutGenerator(seed);
    default_random_engine mutationDecisionGenerator(seed);
    default_random_engine mutationGenerator(seed);
    uniform_real_distribution<float> cutDistribution(0.0, 1.0);
    uniform_real_distribution<float> mutationDecisionDistribution(0.0, 1.0);
    uniform_real_distribution<float> mutationDistribution(-mutationRadius, mutationRadius);

    float startP = cutDistribution(cutGenerator);
    if (startP >= 0.5) {
        // cerr << "Switching starting genome to g2." << endl;
        activeGenome = &g2;
    }

    for (unsigned int i = 0; i < g1.geneWeights.size(); ++i) {
        float pCut = cutDistribution(cutGenerator);
        if (pCut > crossoverThreshold) {
            // Si la variable aleatoria supera el umbral, se hace un corte en los cromosomas.
            // Cuando se hace un corte, se empiezan a copiar alelos del otro cromosoma.
            if (activeGenome == &g1) {
                // cerr << "Switching active genome to g2 at locus " << i << "." << endl;
                activeGenome = &g2;
            } else if (activeGenome == &g2) {
                // cerr << "Switching active genome to g1 at locus " << i << "." << endl;
                activeGenome = &g1;
            }
        }
        float w = activeGenome->geneWeights.at(i);
        float p = mutationDecisionDistribution(mutationDecisionGenerator);
        if (p <= pMutate) {
            // cerr << "The gene on locus " << i << " has mutated." << endl;
            float mutation = mutationDistribution(mutationGenerator);
            w += mutation;
        }
        newWeights.push_back(w);
    }

    Genome result(c, newWeights);
    return result;
}

Genome MatingPool::mitosis(Genome& g1) {
    return crossover(g1, g1);
}

vector<unsigned int> MatingPool::survivorIndices() {
    vector< pair<float, bool> > fitnesses(populationSize);

    vector<unsigned int> result;

    for (unsigned int i = 0; i < populationSize; ++i) {
        // pair means fitness and has_been_used
        fitnesses.at(i) = make_pair(calculateFitness( population.at(i) ), false);
        #ifdef FITNESS
        cerr << currentGeneration << ";" << fitnesses.at(i).first << endl;
        #endif
        #ifdef SHOWSENSEI
        cerr << "Fitness: " << fitnesses.at(i).first << endl;
        #endif
    }

    // Selection sort sin la parte de sort. En cada iteración del ciclo externo,
    // buscamos el elemento de máximo fitness que aún no haya sido elegido
    // y lo agregamos al resultado. Al final, deberíamos tener la cantidad de
    // sobrevivientes especificada y deberían ser los de mayor fitness,
    // ordenados de más adaptado a menos adaptado.

    for (int i = 0; i < amountOfSurvivors; ++i) {
        float max = 0;
        unsigned int bestGenome = 0;

        // For every genome
        for (unsigned int j = 0; j < populationSize; ++j) {
            // If its better than the max and hasnt been used
            if (max < fitnesses.at(j).first && !fitnesses.at(j).second) {
                max = fitnesses.at(j).first;
                bestGenome = j;
            }

        }

        // We have the fittest in bestGenome
        result.push_back(bestGenome);
        // Mark it as used
        fitnesses.at(bestGenome).second = true;
    }

    // cerr << "Survivors ready." << endl;

    return result;
}
