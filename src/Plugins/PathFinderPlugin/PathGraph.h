//#pragma once
//
//#include "fastpathfinder/graph.h"
//
//#include "PathGraphNode.h"
//
//namespace Mengine
//{
//	//////////////////////////////////////////////////////////////////////////
//	class PathGraph
//	{
//	public:
//		void addNode( PathGraphNode * _node, bool _block = false )
//		{
//			m_graph.addNode( _node, _block );
//		}
//
//		void addEdge( PathGraphNode * _from, PathGraphNode * _to, uint32_t _weight )
//		{
//			graph_edge edge;
//			edge.to = _to;
//			edge.weight = _weight;
//
//			_from->edges.push_back( edge );
//		}
//
//		void addEdge2( graph_node * _from, graph_node * _to, uint32_t _weight )
//		{
//			this->addEdge( _from, _to, _weight );
//			this->addEdge( _to, _from, _weight );
//		}
//
//		void blockNode( graph_node * _node, bool _block )
//		{
//			_node->block = _block;
//		}
//
//	public:
//		void getPath( graph_node * _from, graph_node * _to, vector_graph_node & _path )
//		{
//			this->clearWeight_();
//
//			this->wave_( _to, _from, 0 );
//
//			this->makePath_( _from, _path );
//		}
//
//	protected:
//		fastpathfinder::graph m_graph;
//	};
//}
//
