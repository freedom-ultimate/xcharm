#include <iostream>
#include <graph500/graph500_generator.hpp>
#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/timer/timer.hpp>
#include "NDMeshStreamer.h"
#include "DataTypes.h"
#include "Generator.h"
#include "GraphGenerator.decl.h"

namespace GraphLib {

	// GraphGenerator is a C++ wrapper of Charm++ graph generators to make more easily
	// using them in applications 
	template <typename Graph, typename Opts, VertexMapping vertexMapping, GraphType graphType, 
					 GraphGeneratorType graphGeneratorType, TransportType transportType>
	class GraphGenerator;

	// specification for Kronecker graph generator (Charm++) -- TODO: remove this template
	template <typename Graph, typename Opts, VertexMapping vertexMapping, GraphType graphType>
	class GraphGenerator<Graph, Opts, vertexMapping, graphType, GraphGeneratorType::Kronecker, TransportType::Charm> {
		typedef CProxy_Generator_Charm<Graph, Opts, vertexMapping,  graphType, GraphGeneratorType::Kronecker> CProxy_Generator; 
		CProxy_Generator generator;
		public:
			GraphGenerator (Graph & g, Opts & opts) {
				generator = CProxy_Generator::ckNew(g, opts);
			}
			void generate(const CkCallback & cb) { 
				generator.generate(); 
				CkStartQD(cb);
			}
	};

	// specification for Kronecker graph generator (TRAM) -- TODO: remove this template
	template <typename Graph, typename Opts, GraphType graphType>
	class GraphGenerator<Graph, Opts, VertexMapping::SingleVertex, graphType, GraphGeneratorType::Kronecker, TransportType::Tram> {
		typedef typename Graph::Proxy CProxy_Graph;
		typedef typename Graph::Vertex Vertex;
		typedef typename Graph::Edge Edge;
		typedef CProxy_ArrayMeshStreamer<Edge, long long, Vertex, SimpleMeshRouter> CProxy_Aggregator;
		typedef CProxy_Generator_Tram<Graph, Opts, VertexMapping::SingleVertex, graphType, GraphGeneratorType::Kronecker, CProxy_Aggregator> CProxy_Generator; 
		typedef CkIndex_Generator_Tram<Graph, Opts, VertexMapping::SingleVertex, graphType, GraphGeneratorType::Kronecker, CProxy_Aggregator> CkIndex_Generator;
		CProxy_Generator generator;
		CProxy_Aggregator aggregator;
		CProxy_Graph graphProxy;
		Graph g;
		public:
			GraphGenerator (Graph & g, Opts & opts) : g(g), graphProxy(g.getProxy()) {
				const int numMsgsBuffered = 1024;
				int dims[2] = {CkNumNodes(), CkNumPes() / CkNumNodes()};
				aggregator = CProxy_Aggregator::ckNew(numMsgsBuffered, 2, dims, graphProxy, 1);
				generator = CProxy_Generator::ckNew(g /*graphProxy*/, opts, aggregator);
			}
			void generate() { 
				CkCallback startCb(CkIndex_Generator::generate(), generator), fooCb;
				aggregator.init(graphProxy.ckGetArrayID(), startCb, fooCb, -1, true);
			}
	};

	// specification for Kronecker graph generator (TRAM, Multivertex) -- TODO: remove this template
	template <typename Graph, typename Opts, GraphType graphType>
	class GraphGenerator<Graph, Opts, VertexMapping::MultiVertex, graphType, GraphGeneratorType::Kronecker, TransportType::Tram> {
		typedef typename Graph::Proxy CProxy_Graph;
		typedef typename Graph::Vertex Vertex;
		typedef typename Graph::Edge Edge;
		typedef CProxy_ArrayMeshStreamer<std::pair<CmiUInt8, Edge>, long long, Vertex, SimpleMeshRouter> CProxy_Aggregator;
		typedef CProxy_Generator_Tram<Graph, Opts, VertexMapping::MultiVertex, graphType, GraphGeneratorType::Kronecker, CProxy_Aggregator> CProxy_Generator; 
		typedef CkIndex_Generator_Tram<Graph, Opts, VertexMapping::MultiVertex, graphType, GraphGeneratorType::Kronecker, CProxy_Aggregator> CkIndex_Generator;
		CProxy_Generator generator;
		CProxy_Aggregator aggregator;
		CProxy_Graph graphProxy;
		Graph g;
		public:
			GraphGenerator (Graph & g, Opts & opts) : g(g), graphProxy(g.getProxy()) {
				const int numMsgsBuffered = 1024;
				int dims[2] = {CkNumNodes(), CkNumPes() / CkNumNodes()};
				aggregator = CProxy_Aggregator::ckNew(numMsgsBuffered, 2, dims, graphProxy, 1);
				generator = CProxy_Generator::ckNew(g /*graphProxy*/, opts, aggregator);
			}
			void generate() { 
				CkCallback startCb(CkIndex_Generator::generate(), generator), fooCb;
				aggregator.init(graphProxy.ckGetArrayID(), startCb, fooCb, -1, true);
			}
	};

	// specification for SSCA graph generator (Charm++)
	template <typename Graph, typename Opts, VertexMapping vertexMapping, GraphType graphType> 
	class GraphGenerator<Graph, Opts, vertexMapping, graphType, GraphGeneratorType::SSCA, TransportType::Charm> {
		typedef CProxy_SSCA_Generator_Charm<Graph, Opts, vertexMapping,  graphType> CProxy_Generator; 
		CProxy_Generator generator;
		public:
			GraphGenerator (Graph & g, Opts & opts) {
				generator = CProxy_Generator::ckNew(g, opts);
			}
			void generate(const CkCallback & cb) { 
				generator.generate(cb); 
			}
	};

	// specification for SSCA graph generator (TRAM)
	template <typename Graph, typename Opts, VertexMapping vertexMapping, GraphType graphType> 
	class GraphGenerator<Graph, Opts, vertexMapping, graphType, GraphGeneratorType::SSCA, TransportType::Tram> {
		typedef typename Graph::Proxy CProxy_Graph;
		typedef typename Graph::Vertex Vertex;
		typedef typename Graph::Edge Edge;
		typedef CProxy_ArrayMeshStreamer<Edge, long long, Vertex, SimpleMeshRouter> CProxy_Aggregator;
		typedef CProxy_SSCA_Generator_Tram<Graph, Opts, vertexMapping, graphType, CProxy_Aggregator> CProxy_Generator; 
		typedef CkIndex_SSCA_Generator_Tram<Graph, Opts, vertexMapping, graphType, CProxy_Aggregator> CkIndex_Generator;
		CProxy_Generator generator;
		CProxy_Aggregator aggregator;
		CProxy_Graph graphProxy;
		Graph g;
		public:
			GraphGenerator (Graph & g, Opts & opts) : g(g), graphProxy(g.getProxy()) {
				const int numMsgsBuffered = 1024;
				int dims[2] = {CkNumNodes(), CkNumPes() / CkNumNodes()};
				aggregator = CProxy_Aggregator::ckNew(numMsgsBuffered, 2, dims, graphProxy, 1);
				generator = CProxy_Generator::ckNew(g /*graphProxy*/, opts, aggregator);
			}
			void generate(const CkCallback & cb) { 
				aggregator.init(-1);
				generator.generate(cb);
			}
	};

	// specification for RMAT graph generator (Charm++)
	template <typename Graph, typename Opts, VertexMapping vertexMapping, GraphType graphType> 
	class GraphGenerator<Graph, Opts, vertexMapping, graphType, GraphGeneratorType::RMAT, TransportType::Charm> {
		typedef CProxy_RMAT_Generator_Charm<Graph, Opts, vertexMapping,  graphType> CProxy_Generator; 
		CProxy_Generator generator;
		public:
			GraphGenerator (Graph & g, Opts & opts) {
				generator = CProxy_Generator::ckNew(g, opts);
			}
			void generate(const CkCallback & cb) { generator.generate(cb); }
	};

	// specification for RMAT graph generator (TRAM)
	template <typename Graph, typename Opts, VertexMapping vertexMapping, GraphType graphType> 
	class GraphGenerator<Graph, Opts, vertexMapping, graphType, GraphGeneratorType::RMAT, TransportType::Tram> {
		typedef typename Graph::Proxy CProxy_Graph;
		typedef typename Graph::Vertex Vertex;
		typedef typename Graph::Edge Edge;
		typedef CProxy_ArrayMeshStreamer<Edge, long long, Vertex, SimpleMeshRouter> CProxy_Aggregator;
		typedef CProxy_RMAT_Generator_Tram<Graph, Opts, vertexMapping, graphType, CProxy_Aggregator> CProxy_Generator; 
		typedef CkIndex_RMAT_Generator_Tram<Graph, Opts, vertexMapping, graphType, CProxy_Aggregator> CkIndex_Generator;
		CProxy_Generator generator;
		CProxy_Aggregator aggregator;
		CProxy_Graph graphProxy;
		Graph g;
		public:
			GraphGenerator (Graph & g, Opts & opts) : g(g), graphProxy(g.getProxy()) {
				const int numMsgsBuffered = 1024;
				int dims[2] = {CkNumNodes(), CkNumPes() / CkNumNodes()};
				aggregator = CProxy_Aggregator::ckNew(numMsgsBuffered, 2, dims, graphProxy, 1);
				generator = CProxy_Generator::ckNew(g /*graphProxy*/, opts, aggregator);
			}
			void generate(const CkCallback & cb) { 
				aggregator.init(-1);
				generator.generate(cb);
			}
	};

}
#include "OldGenerators.h"

#define CK_TEMPLATES_ONLY
#include "GraphGenerator.def.h"
#undef CK_TEMPLATES_ONLY

