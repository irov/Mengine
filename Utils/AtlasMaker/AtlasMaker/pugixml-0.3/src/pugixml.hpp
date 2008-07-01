///////////////////////////////////////////////////////////////////////////////
//
// Pug Improved XML Parser - Version 0.3
// --------------------------------------------------------
// Copyright (C) 2006-2007, by Arseny Kapoulkine (arseny.kapoulkine@gmail.com)
// This work is based on the pugxml parser, which is:
// Copyright (C) 2003, by Kristen Wegner (kristen@tima.net)
// Released into the Public Domain. Use at your own risk.
// See pugxml.xml for further information, history, etc.
// Contributions by Neville Franks (readonly@getsoft.com).
//
///////////////////////////////////////////////////////////////////////////////

#ifndef HEADER_PUGIXML_HPP
#define HEADER_PUGIXML_HPP

#include "pugiconfig.hpp"

#ifndef PUGIXML_NO_STL
#	include <string>
#	include <istream>
#	include <exception>
#endif

// No XPath without STL
#ifdef PUGIXML_NO_STL
#	ifndef PUGIXML_NO_XPATH
#		define PUGIXML_NO_XPATH
#	endif
#endif

#include <cstddef>
#include <cstring>

/// The PugiXML Parser namespace.
namespace pugi
{
	/// Tree node classification.
	enum xml_node_type
	{
		node_null,			///< Undifferentiated entity
		node_document,		///< A document tree's absolute root.
		node_element,		///< E.g. '<...>'
		node_pcdata,		///< E.g. '>...<'
		node_cdata,			///< E.g. '<![CDATA[...]]>'
		node_comment,		///< E.g. '<!--...-->'
		node_pi				///< E.g. '<?...?>'
	};

	// Parsing options

	/**
	 * Memory block size, used for fast allocator. Memory for DOM tree is allocated in blocks of
	 * memory_block_size + 4.
	 * This value affects size of xml_memory class.
	 */
	const size_t memory_block_size = 32768;

	/**
	 * Minimal parsing mode. Equivalent to turning all other flags off. This set of flags means
	 * that pugixml does not add pi/cdata sections or comments to DOM tree and does not perform
	 * any conversions for input data, meaning fastest parsing.
	 */
	const unsigned int parse_minimal			= 0x0000;

	/**
	 * This flag determines if processing instructions (nodes with type node_pi; such nodes have the
	 * form of <? target content ?> or <? target ?> in XML) are to be put in DOM tree. If this flag is off,
	 * they are not put in the tree, but are still parsed and checked for correctness.
	 *
	 * The corresponding node in DOM tree will have type node_pi, name "target" and value "content",
	 * if any.
	 *
	 * Note that <?xml ...?> (document declaration) is not considered to be a PI.
	 *
	 * This flag is off by default.
	 */
	const unsigned int parse_pi					= 0x0001;

	/**
	 * This flag determines if comments (nodes with type node_comment; such nodes have the form of
	 * <!-- content --> in XML) are to be put in DOM tree. If this flag is off, they are not put in
	 * the tree, but are still parsed and checked for correctness.
	 *
	 * The corresponding node in DOM tree will have type node_comment, empty name and value "content".
	 *
	 * This flag is off by default.
	 */
	const unsigned int parse_comments			= 0x0002;

	/**
	 * This flag determines if CDATA sections (nodes with type node_cdata; such nodes have the form
	 * of <![CDATA[[content]]> in XML) are to be put in DOM tree. If this flag is off, they are not
	 * put in the tree, but are still parsed and checked for correctness.
	 *
	 * The corresponding node in DOM tree will have type node_cdata, empty name and value "content".
	 *
	 * This flag is on by default.
	 */
	const unsigned int parse_cdata				= 0x0004;

	/**
	 * This flag determines if nodes with PCDATA (regular text) that consist only of whitespace
	 * characters are to be put in DOM tree. Often whitespace-only data is not significant for the
	 * application, and the cost of allocating and storing such nodes (both memory and speed-wise)
	 * can be significant. For example, after parsing XML string "<node> <a/> </node>", <node> element
	 * will have 3 children when parse_ws_pcdata is set (child with type node_pcdata and value=" ",
	 * child with type node_element and name "a", and another child with type node_pcdata and
	 * value=" "), and only 1 child when parse_ws_pcdata is not set.
	 * 
	 * This flag is off by default.
	 */
	const unsigned int parse_ws_pcdata			= 0x0008;

	/**
	 * This flag determines if character and entity references are to be expanded during the parsing
	 * process. Character references are &amp;#...; or &amp;#x...; (... is Unicode numeric representation of
     * character in either decimal (&amp;#...;) or hexadecimal (&amp;#x...;) form), entity references are &amp;...;
     * Note that as pugixml does not handle DTD, the only allowed entities are predefined ones - 
     * &amp;lt;, &amp;gt;, &amp;amp;, &amp;apos; and &amp;quot;. If character/entity reference can not be expanded, it is
     * leaved as is, so you can do additional processing later.
     * Reference expansion is performed in attribute values and PCDATA content.
     *
     * This flag is on by default.
     */
	const unsigned int parse_escapes			= 0x0010;

	/**
	 * This flag determines if EOL handling (that is, replacing sequences 0x0d 0x0a by a single 0x0a
	 * character, and replacing all standalone 0x0d characters by 0x0a) is to be performed on input
	 * data (that is, comments contents, PCDATA/CDATA contents and attribute values).
	 *
	 * This flag is on by default.
	 */
	const unsigned int parse_eol				= 0x0020;
	
 	/**
 	 * This flag determines if attribute value normalization should be performed for all attributes,
 	 * assuming that their type is not CDATA. This means, that:
 	 * 1. Whitespace characters (new line, tab and space) are replaced with space (' ')
 	 * 2. Afterwards sequences of spaces are replaced with a single space
 	 * 3. Leading/trailing whitespace characters are trimmed
 	 * 
 	 * This flag is off by default.
 	 */
 	const unsigned int parse_wnorm_attribute	= 0x0040;

 	/**
 	 * This flag determines if attribute value normalization should be performed for all attributes,
 	 * assuming that their type is CDATA. This means, that whitespace characters (new line, tab and
 	 * space) are replaced with space (' '). Note, that the actions performed while this flag is on
 	 * are also performed if parse_wnorm_attribute is on, so this flag has no effect if
 	 * parse_wnorm_attribute flag is set.
 	 * 
 	 * This flag is on by default.
 	 */
 	const unsigned int parse_wconv_attribute	= 0x0080;
	
	/**
     * This is the default set of flags. It includes parsing CDATA sections (comments/PIs are not
     * parsed), performing character and entity reference expansion, replacing whitespace characters
     * with spaces in attribute values and performing EOL handling. Note, that PCDATA sections
     * consisting only of whitespace characters are not parsed (by default) for performance reasons.
     */
	const unsigned int parse_default			= parse_cdata | parse_escapes | parse_wconv_attribute | parse_eol;

	// Formatting flags
	
	/**
	 * Indent the nodes that are written to output stream with as many indentation strings as deep
	 * the node is in DOM tree.
	 *
	 * This flag is on by default.
	 */
	const unsigned int format_indent	= 0x01;
	
	/**
	 * This flag determines how the non-printable symbols are written to output stream - they are
	 * either considered UTF-8 and are written as UTF-8 character, escaped with &#...;, or they are
	 * considered to be ASCII and each ASCII character is escaped separately.
	 *
	 * This flag is on by default.
	 */
	const unsigned int format_utf8		= 0x02;
	
	/**
	 * This flag determines if UTF-8 BOM is to be written to output stream.
	 *
	 * This flag is off by default.
	 */
	const unsigned int format_write_bom	= 0x04;
	
	/**
	 * If this flag is on, no indentation is performed and no line breaks are written to output file.
	 * This means that the data is written to output stream as is.
	 *
	 * This flag is off by default.
	 */
	const unsigned int format_raw		= 0x08;
	
	/**
	 * This is the default set of formatting flags. It includes indenting nodes depending on their
	 * depth in DOM tree and considering input data to be UTF-8.
	 */
	const unsigned int format_default	= format_indent | format_utf8;
		
	// Forward declarations
	struct xml_attribute_struct;
	struct xml_node_struct;

	class xml_allocator;

	class xml_node_iterator;
	class xml_attribute_iterator;

	class xml_tree_walker;
	
	class xml_node;

	#ifndef PUGIXML_NO_XPATH
	class xpath_node;
	class xpath_node_set;
	class xpath_ast_node;
	class xpath_allocator;
	
	/**
	 * A class that holds compiled XPath query and allows to evaluate query result
	 */
	class xpath_query
	{
	private:
		// Noncopyable semantics
		xpath_query(const xpath_query&);
		xpath_query& operator=(const xpath_query&);

		xpath_allocator* m_alloc;
		xpath_ast_node* m_root;

		void compile(const char* query);

	public:
		/**
		 * Ctor from string with XPath expression.
		 * Throws xpath_exception on compilation error, std::bad_alloc on out of memory error.
		 *
		 * \param query - string with XPath expression
		 */
		explicit xpath_query(const char* query);

		/**
		 * Dtor
		 */
		~xpath_query();
		
		/**
		 * Evaluate expression as boolean value for the context node \a n.
		 * If expression does not directly evaluate to boolean, the expression result is converted
		 * as through boolean() XPath function call.
		 * Throws std::bad_alloc on out of memory error.
		 *
		 * \param n - context node
		 * \return evaluation result
		 */
		bool evaluate_boolean(const xml_node& n);
		
		/**
		 * Evaluate expression as double value for the context node \a n.
		 * If expression does not directly evaluate to double, the expression result is converted
		 * as through number() XPath function call.
		 * Throws std::bad_alloc on out of memory error.
		 *
		 * \param n - context node
		 * \return evaluation result
		 */
		double evaluate_number(const xml_node& n);
		
		/**
		 * Evaluate expression as string value for the context node \a n.
		 * If expression does not directly evaluate to string, the expression result is converted
		 * as through string() XPath function call.
		 * Throws std::bad_alloc on out of memory error.
		 *
		 * \param n - context node
		 * \return evaluation result
		 */
		std::string evaluate_string(const xml_node& n);
		
		/**
		 * Evaluate expression as node set for the context node \a n.
		 * If expression does not directly evaluate to node set, function returns empty node set.
		 * Throws std::bad_alloc on out of memory error.
		 *
		 * \param n - context node
		 * \return evaluation result
		 */
		xpath_node_set evaluate_node_set(const xml_node& n);
	};
	#endif
	
	/**
	 * A light-weight wrapper for manipulating attributes in DOM tree.
	 * Note: xml_attribute does not allocate any memory for the attribute it wraps; it only wraps a
	 * pointer to existing attribute.
	 */
	class xml_attribute
	{
		friend class xml_attribute_iterator;
		friend class xml_node;

	private:
		xml_attribute_struct* _attr;
	
    	/// \internal Safe bool type
    	typedef xml_attribute_struct* xml_attribute::*unspecified_bool_type;

		/// \internal Initializing ctor
		explicit xml_attribute(xml_attribute_struct* attr);

	public:
		/**
		 * Default ctor. Constructs an empty attribute.
		 */
		xml_attribute();
		
	public:
    	/**
    	 * Safe bool conversion.
    	 * Allows xml_node to be used in a context where boolean variable is expected, such as 'if (node)'.
    	 */
    	operator unspecified_bool_type() const;

    	// Borland C++ workaround
    	bool operator!() const;

		/**
		 * Compare wrapped pointer to the attribute to the pointer that is wrapped by \a r.
		 *
		 * \param r - value to compare to
		 * \return comparison result
		 */
		bool operator==(const xml_attribute& r) const;
		
		/**
		 * Compare wrapped pointer to the attribute to the pointer that is wrapped by \a r.
		 *
		 * \param r - value to compare to
		 * \return comparison result
		 */
		bool operator!=(const xml_attribute& r) const;
		
		/**
		 * Compare wrapped pointer to the attribute to the pointer that is wrapped by \a r.
		 *
		 * \param r - value to compare to
		 * \return comparison result
		 */
		bool operator<(const xml_attribute& r) const;
		
		/**
		 * Compare wrapped pointer to the attribute to the pointer that is wrapped by \a r.
		 *
		 * \param r - value to compare to
		 * \return comparison result
		 */
		bool operator>(const xml_attribute& r) const;
		
		/**
		 * Compare wrapped pointer to the attribute to the pointer that is wrapped by \a r.
		 *
		 * \param r - value to compare to
		 * \return comparison result
		 */
		bool operator<=(const xml_attribute& r) const;
		
		/**
		 * Compare wrapped pointer to the attribute to the pointer that is wrapped by \a r.
		 *
		 * \param r - value to compare to
		 * \return comparison result
		 */
		bool operator>=(const xml_attribute& r) const;

	public:
    	/**
    	 * Get next attribute in attribute list of node that contains the attribute.
    	 *
    	 * \return next attribute, if any; empty attribute otherwise
    	 */
    	xml_attribute next_attribute() const;

    	/**
    	 * Get previous attribute in attribute list of node that contains the attribute.
    	 *
    	 * \return previous attribute, if any; empty attribute otherwise
    	 */
    	xml_attribute previous_attribute() const;

		/**
		 * Cast attribute value as int.
		 *
		 * \return attribute value as int, or 0 if conversion did not succeed or attribute is empty
		 */
		int as_int() const;

		/**
		 * Cast attribute value as double.
		 *
		 * \return attribute value as double, or 0.0 if conversion did not succeed or attribute is empty
		 */
		double as_double() const;
	
		/**
		 * Cast attribute value as float.
		 *
		 * \return attribute value as float, or 0.0f if conversion did not succeed or attribute is empty
		 */
		float as_float() const;

		/**
		 * Cast attribute value as bool. Returns true for attributes with values that start with '1',
		 * 't', 'T', 'y', 'Y', returns false for other attributes.
		 *
		 * \return attribute value as bool, or false if conversion did not succeed or attribute is empty
		 */
		bool as_bool() const;

		/// \internal Document order or 0 if not set
		unsigned int document_order() const;

	public:
		/**
         * Set attribute value to \a rhs.
         *
         * \param rhs - new attribute value
         * \return self
         */
		xml_attribute& operator=(const char* rhs);
	
		/**
         * Set attribute value to \a rhs.
         *
         * \param rhs - new attribute value
         * \return self
         */
		xml_attribute& operator=(int rhs);
	
		/**
         * Set attribute value to \a rhs.
         *
         * \param rhs - new attribute value
         * \return self
         */
		xml_attribute& operator=(double rhs);
		
		/**
         * Set attribute value to either 'true' or 'false' (depends on whether \a rhs is true or false).
         *
         * \param rhs - new attribute value
         * \return self
         */
		xml_attribute& operator=(bool rhs);

		/**
		 * Set attribute name to \a rhs.
		 *
		 * \param rhs - new attribute name
		 * \return success flag (call fails if attribute is empty or there is not enough memory)
		 */
		bool set_name(const char* rhs);
		
		/**
		 * Set attribute value to \a rhs.
		 *
		 * \param rhs - new attribute value
		 * \return success flag (call fails if attribute is empty or there is not enough memory)
		 */
		bool set_value(const char* rhs);

	public:
		/**
		 * Check if attribute is empty.
		 *
		 * \return true if attribute is empty, false otherwise
		 */
		bool empty() const;

	public:
		/**
		 * Get attribute name.
		 *
		 * \return attribute name, or "" if attribute is empty
		 */
		const char* name() const;

		/**
		 * Get attribute value.
		 *
		 * \return attribute value, or "" if attribute is empty
		 */
		const char* value() const;
	};

#ifdef __BORLANDC__
	// Borland C++ workaround
	bool operator&&(const xml_attribute& lhs, bool rhs);
	bool operator||(const xml_attribute& lhs, bool rhs);
#endif

	/**
	 * A light-weight wrapper for manipulating nodes in DOM tree.
	 * Note: xml_node does not allocate any memory for the node it wraps; it only wraps a pointer to
	 * existing node.
	 */
	class xml_node
	{
		friend class xml_node_iterator;

	protected:
		xml_node_struct* _root;

    	/// \internal Safe bool type
    	typedef xml_node_struct* xml_node::*unspecified_bool_type;

		/// \internal Initializing ctor
		explicit xml_node(xml_node_struct* p);

		/// \internal Precompute document order (valid only for document node)
		void precompute_document_order_impl();

		/// \internal Get allocator
		xml_allocator& get_allocator() const;

	public:
		/**
		 * Default ctor. Constructs an empty node.
		 */
		xml_node();

	public:
    	/**
    	 * Safe bool conversion.
    	 * Allows xml_node to be used in a context where boolean variable is expected, such as 'if (node)'.
    	 */
		operator unspecified_bool_type() const;

		// Borland C++ workaround
		bool operator!() const;
	
		/**
		 * Compare wrapped pointer to the attribute to the pointer that is wrapped by \a r.
		 *
		 * \param r - value to compare to
		 * \return comparison result
		 */
		bool operator==(const xml_node& r) const;

		/**
		 * Compare wrapped pointer to the attribute to the pointer that is wrapped by \a r.
		 *
		 * \param r - value to compare to
		 * \return comparison result
		 */
		bool operator!=(const xml_node& r) const;

		/**
		 * Compare wrapped pointer to the attribute to the pointer that is wrapped by \a r.
		 *
		 * \param r - value to compare to
		 * \return comparison result
		 */
		bool operator<(const xml_node& r) const;

		/**
		 * Compare wrapped pointer to the attribute to the pointer that is wrapped by \a r.
		 *
		 * \param r - value to compare to
		 * \return comparison result
		 */
		bool operator>(const xml_node& r) const;

		/**
		 * Compare wrapped pointer to the attribute to the pointer that is wrapped by \a r.
		 *
		 * \param r - value to compare to
		 * \return comparison result
		 */
		bool operator<=(const xml_node& r) const;
		
		/**
		 * Compare wrapped pointer to the attribute to the pointer that is wrapped by \a r.
		 *
		 * \param r - value to compare to
		 * \return comparison result
		 */
		bool operator>=(const xml_node& r) const;

	public:
		/**
		 * Node iterator type (for child nodes).
		 * \see xml_node_iterator
		 */
		typedef xml_node_iterator iterator;

		/**
		 * Node iterator type (for child nodes).
		 * \see xml_attribute_iterator
		 */
		typedef xml_attribute_iterator attribute_iterator;

		/**
		 * Access the begin iterator for this node's collection of child nodes.
		 *
		 * \return iterator that points to the first child node, or past-the-end iterator if node is empty or has no children
		 */
		iterator begin() const;
	
		/**
		 * Access the end iterator for this node's collection of child nodes.
		 *
		 * \return past-the-end iterator for child list
		 */
		iterator end() const;
	
		/**
		 * Access the begin iterator for this node's collection of attributes.
		 *
		 * \return iterator that points to the first attribute, or past-the-end iterator if node is empty or has no attributes
		 */
		attribute_iterator attributes_begin() const;
	
		/**
		 * Access the end iterator for this node's collection of attributes.
		 *
		 * \return past-the-end iterator for attribute list
		 */
		attribute_iterator attributes_end() const;

	public:
		/**
		 * Check if node is empty.
		 *
		 * \return true if node is empty, false otherwise
		 */
		bool empty() const;

	public:
		/**
		 * Get node type
		 *
		 * \return node type; node_null for empty nodes
		 */
		xml_node_type type() const;

		/**
		 * Get node name (element name for element nodes, PI target for PI)
		 *
		 * \return node name, if any; "" otherwise
		 */
		const char* name() const;

		/**
		 * Get node value (comment/PI/PCDATA/CDATA contents, depending on node type)
		 *
		 * \return node value, if any; "" otherwise
		 */
		const char* value() const;
	
		/**
		 * Get child with the specified name
		 *
		 * \param name - child name
		 * \return child with the specified name, if any; empty node otherwise
		 */
		xml_node child(const char* name) const;

		/**
		 * Get child with the name that matches specified pattern
		 *
		 * \param name - child name pattern
		 * \return child with the name that matches pattern, if any; empty node otherwise
		 */
		xml_node child_w(const char* name) const;

		/**
		 * Get attribute with the specified name
		 *
		 * \param name - attribute name
		 * \return attribute with the specified name, if any; empty attribute otherwise
		 */
		xml_attribute attribute(const char* name) const;

		/**
		 * Get attribute with the name that matches specified pattern
		 *
		 * \param name - attribute name pattern
		 * \return attribute with the name that matches pattern, if any; empty attribute otherwise
		 */
		xml_attribute attribute_w(const char* name) const;

		/**
		 * Get first of following sibling nodes with the specified name
		 *
		 * \param name - sibling name
		 * \return node with the specified name, if any; empty node otherwise
		 */
		xml_node next_sibling(const char* name) const;

		/**
		 * Get first of the following sibling nodes with the name that matches specified pattern
		 *
		 * \param name - sibling name pattern
		 * \return node with the name that matches pattern, if any; empty node otherwise
		 */
		xml_node next_sibling_w(const char* name) const;

		/**
		 * Get following sibling
		 *
		 * \return following sibling node, if any; empty node otherwise
		 */
		xml_node next_sibling() const;

		/**
		 * Get first of preceding sibling nodes with the specified name
		 *
		 * \param name - sibling name
		 * \return node with the specified name, if any; empty node otherwise
		 */
		xml_node previous_sibling(const char* name) const;

		/**
		 * Get first of the preceding sibling nodes with the name that matches specified pattern
		 *
		 * \param name - sibling name pattern
		 * \return node with the name that matches pattern, if any; empty node otherwise
		 */
		xml_node previous_sibling_w(const char* name) const;

		/**
		 * Get preceding sibling
		 *
		 * \return preceding sibling node, if any; empty node otherwise
		 */
		xml_node previous_sibling() const;

		/**
		 * Get parent node
		 *
		 * \return parent node if any; empty node otherwise
		 */
		xml_node parent() const;

		/**
		 * Get root of DOM tree this node belongs to.
		 *
		 * \return tree root
		 */
		xml_node root() const;

		/**
		 * Get child value of current node; that is, value of the first child node of type PCDATA/CDATA
		 *
		 * \return child value of current node, if any; "" otherwise
		 */
		const char* child_value() const;

		/**
		 * Get child value of child with specified name. \see child_value
		 * node.child_value(name) is equivalent to node.child(name).child_value()
		 *
		 * \param name - child name
		 * \return child value of specified child node, if any; "" otherwise
		 */
		const char* child_value(const char* name) const;

		/**
		 * Get child value of child with name that matches the specified pattern. \see child_value
		 * node.child_value_w(name) is equivalent to node.child_w(name).child_value()
		 *
		 * \param name - child name pattern
		 * \return child value of specified child node, if any; "" otherwise
		 */
		const char* child_value_w(const char* name) const;

	public:	
		/**
		 * Set node name to \a rhs (for PI/element nodes). \see name
		 *
		 * \param rhs - new node name
		 * \return success flag (call fails if node is of the wrong type or there is not enough memory)
		 */
		bool set_name(const char* rhs);
		
		/**
		 * Set node value to \a rhs (for PI/PCDATA/CDATA/comment nodes). \see value
		 *
		 * \param rhs - new node value
		 * \return success flag (call fails if node is of the wrong type or there is not enough memory)
		 */
		bool set_value(const char* rhs);

		/**
		 * Add attribute with specified name (for element nodes)
		 *
		 * \param name - attribute name
		 * \return added attribute, or empty attribute if there was an error (wrong node type)
		 */
		xml_attribute append_attribute(const char* name);

		/**
		 * Insert attribute with specified name after \a attr (for element nodes)
		 *
		 * \param name - attribute name
		 * \param attr - attribute to insert a new one after
		 * \return inserted attribute, or empty attribute if there was an error (wrong node type, or attr does not belong to node)
		 */
		xml_attribute insert_attribute_after(const char* name, const xml_attribute& attr);

		/**
		 * Insert attribute with specified name before \a attr (for element nodes)
		 *
		 * \param name - attribute name
		 * \param attr - attribute to insert a new one before
		 * \return inserted attribute, or empty attribute if there was an error (wrong node type, or attr does not belong to node)
		 */
		xml_attribute insert_attribute_before(const char* name, const xml_attribute& attr);

		/**
		 * Add child node with specified type (for element nodes)
		 *
		 * \param type - node type
		 * \return added node, or empty node if there was an error (wrong node type)
		 */
		xml_node append_child(xml_node_type type = node_element);

		/**
		 * Insert child node with specified type after \a node (for element nodes)
		 *
		 * \param type - node type
		 * \param node - node to insert a new one after
		 * \return inserted node, or empty node if there was an error (wrong node type, or \a node is not a child of this node)
		 */
		xml_node insert_child_after(xml_node_type type, const xml_node& node);

		/**
		 * Insert child node with specified type before \a node (for element nodes)
		 *
		 * \param type - node type
		 * \param node - node to insert a new one before
		 * \return inserted node, or empty node if there was an error (wrong node type, or \a node is not a child of this node)
		 */
		xml_node insert_child_before(xml_node_type type, const xml_node& node);

		/**
		 * Remove specified attribute
		 *
		 * \param a - attribute to be removed
		 */
		void remove_attribute(const xml_attribute& a);

		/**
		 * Remove attribute with the specified name, if any
		 *
		 * \param name - attribute name
		 */
		void remove_attribute(const char* name);

		/**
		 * Remove specified child
		 *
		 * \param n - child node to be removed
		 */
		void remove_child(const xml_node& n);

		/**
		 * Remove child with the specified name, if any
		 *
		 * \param name - child name
		 */
		void remove_child(const char* name);

	public:
		/**
		 * Get first attribute
		 *
		 * \return first attribute, if any; empty attribute otherwise
		 */
		xml_attribute first_attribute() const;

		/**
		 * Get last attribute
		 *
		 * \return last attribute, if any; empty attribute otherwise
		 */
        xml_attribute last_attribute() const;

		/**
		 * Get all elements from subtree with given name
		 *
		 * \param name - node name
		 * \param it - output iterator (for example, std::back_insert_iterator (result of std::back_inserter))
		 */
		template <typename OutputIterator> void all_elements_by_name(const char* name, OutputIterator it) const;

		/**
		 * Get all elements from subtree with name that matches given pattern
		 *
		 * \param name - node name pattern
		 * \param it - output iterator (for example, std::back_insert_iterator (result of std::back_inserter))
		 */
		template <typename OutputIterator> void all_elements_by_name_w(const char* name, OutputIterator it) const;

		/**
		 * Get first child
		 *
		 * \return first child, if any; empty node otherwise
		 */
		xml_node first_child() const;

		/**
		 * Get last child
		 *
		 * \return last child, if any; empty node otherwise
		 */
        xml_node last_child() const;
		
		/**
		 * Find attribute using predicate
		 *
		 * \param pred - predicate, that takes xml_attribute and returns bool
		 * \return first attribute for which predicate returned true, or empty attribute
		 */
		template <typename Predicate> xml_attribute find_attribute(Predicate pred) const;

		/**
		 * Find child node using predicate
		 *
		 * \param pred - predicate, that takes xml_node and returns bool
		 * \return first child node for which predicate returned true, or empty node
		 */
		template <typename Predicate> xml_node find_child(Predicate pred) const;

		/**
		 * Find node from subtree using predicate
		 *
		 * \param pred - predicate, that takes xml_node and returns bool
		 * \return first node from subtree for which predicate returned true, or empty node
		 */
		template <typename Predicate> xml_node find_node(Predicate pred) const;

#ifndef PUGIXML_NO_STL
		/**
		 * Get the absolute node path from root as a text string.
		 *
		 * \param delimiter - delimiter character to insert between element names
		 * \return path string (e.g. '/bookstore/book/author').
		 */
		std::string path(char delimiter = '/') const;
#endif

		/**
		 * Search for a node by path.
		 * \param path - path string; e.g. './foo/bar' (relative to node), '/foo/bar' (relative 
		 * to root), '../foo/bar'.
		 * \param delimiter - delimiter character to use while tokenizing path
		 * \return matching node, if any; empty node otherwise
		 */
		xml_node first_element_by_path(const char* path, char delimiter = '/') const;

		/**
		 * Recursively traverse subtree with xml_tree_walker
		 * \see xml_tree_walker::begin
		 * \see xml_tree_walker::for_each
		 * \see xml_tree_walker::end
		 *
		 * \param walker - tree walker to traverse subtree with
		 * \return traversal result
		 */
		bool traverse(xml_tree_walker& walker);
	
	#ifndef PUGIXML_NO_XPATH
		/**
		 * Select single node by evaluating XPath query
		 * 
		 * \param query - query string
		 * \return first node from the resulting node set by document order, or empty node if none found
		 */
		xpath_node select_single_node(const char* query) const;

		/**
		 * Select single node by evaluating XPath query
		 *
		 * \param query - compiled query
		 * \return first node from the resulting node set by document order, or empty node if none found
		 */
		xpath_node select_single_node(xpath_query& query) const;

		/**
		 * Select node set by evaluating XPath query
		 *
		 * \param query - query string
		 * \return resulting node set
		 */
		xpath_node_set select_nodes(const char* query) const;

		/**
		 * Select node set by evaluating XPath query
		 *
		 * \param query - compiled query
		 * \return resulting node set
		 */
		xpath_node_set select_nodes(xpath_query& query) const;
	#endif
		
		/// \internal Document order or 0 if not set
		unsigned int document_order() const;

	#ifndef PUGIXML_NO_STL
		/**
		 * Print subtree to stream
		 *
		 * \param os - output stream
		 * \param indent - indentation string
		 * \param flags - formatting flags
		 * \param depth - starting depth (used for indentation)
		 */
		void print(std::ostream& os, const char* indent = "\t", unsigned int flags = format_default, unsigned int depth = 0);
	#endif
	};

#ifdef __BORLANDC__
	// Borland C++ workaround
	bool operator&&(const xml_node& lhs, bool rhs);
	bool operator||(const xml_node& lhs, bool rhs);
#endif

	/**
	 * Child node iterator.
	 * It's a bidirectional iterator with value type 'xml_node'.
	 */
	class xml_node_iterator
#ifndef PUGIXML_NO_STL
	: public std::iterator<std::bidirectional_iterator_tag, xml_node>
#endif
	{
		friend class xml_node;

	private:
		xml_node _prev;
		xml_node _wrap;

		/// \internal Initializing ctor
		explicit xml_node_iterator(xml_node_struct* ref);

	public:
		/**
		 * Default ctor
		 */
		xml_node_iterator();

		/**
		 * Initializing ctor
		 *
		 * \param node - node that iterator will point at
		 */
		xml_node_iterator(const xml_node& node);

		/**
		 * Initializing ctor (for past-the-end)
		 *
		 * \param ref - should be 0
		 * \param prev - previous node
		 */
		xml_node_iterator(xml_node_struct* ref, xml_node_struct* prev);

		/**
		 * Check if this iterator is equal to \a rhs
		 *
		 * \param rhs - other iterator
		 * \return comparison result
		 */
		bool operator==(const xml_node_iterator& rhs) const;
		
		/**
		 * Check if this iterator is not equal to \a rhs
		 *
		 * \param rhs - other iterator
		 * \return comparison result
		 */
		bool operator!=(const xml_node_iterator& rhs) const;

		/**
		 * Dereferencing operator
		 *
		 * \return reference to the node iterator points at
		 */
		xml_node& operator*();

		/**
		 * Member access operator
		 *
		 * \return poitner to the node iterator points at
		 */
		xml_node* operator->();

		/**
		 * Pre-increment operator
		 *
		 * \return self
		 */
		const xml_node_iterator& operator++();

		/**
		 * Post-increment operator
		 *
		 * \return old value
		 */
		xml_node_iterator operator++(int);
		
		/**
		 * Pre-decrement operator
		 *
		 * \return self
		 */
		const xml_node_iterator& operator--();
		
		/**
		 * Post-decrement operator
		 *
		 * \return old value
		 */
		xml_node_iterator operator--(int);
	};

	/**
	 * Attribute iterator.
	 * It's a bidirectional iterator with value type 'xml_attribute'.
	 */
	class xml_attribute_iterator
#ifndef PUGIXML_NO_STL
	: public std::iterator<std::bidirectional_iterator_tag, xml_attribute>
#endif
	{
		friend class xml_node;

	private:
		xml_attribute _prev;
		xml_attribute _wrap;

		/// \internal Initializing ctor
		explicit xml_attribute_iterator(xml_attribute_struct* ref);

	public:
		/**
		 * Default ctor
		 */
		xml_attribute_iterator();

		/**
		 * Initializing ctor
		 *
		 * \param node - node that iterator will point at
		 */
		xml_attribute_iterator(const xml_attribute& node);

		/**
		 * Initializing ctor (for past-the-end)
		 *
		 * \param ref - should be 0
		 * \param prev - previous node
		 */
		xml_attribute_iterator(xml_attribute_struct* ref, xml_attribute_struct* prev);

		/**
		 * Check if this iterator is equal to \a rhs
		 *
		 * \param rhs - other iterator
		 * \return comparison result
		 */
		bool operator==(const xml_attribute_iterator& rhs) const;
		
		/**
		 * Check if this iterator is not equal to \a rhs
		 *
		 * \param rhs - other iterator
		 * \return comparison result
		 */
		bool operator!=(const xml_attribute_iterator& rhs) const;

		/**
		 * Dereferencing operator
		 *
		 * \return reference to the node iterator points at
		 */
		xml_attribute& operator*();

		/**
		 * Member access operator
		 *
		 * \return poitner to the node iterator points at
		 */
		xml_attribute* operator->();

		/**
		 * Pre-increment operator
		 *
		 * \return self
		 */
		const xml_attribute_iterator& operator++();

		/**
		 * Post-increment operator
		 *
		 * \return old value
		 */
		xml_attribute_iterator operator++(int);
		
		/**
		 * Pre-decrement operator
		 *
		 * \return self
		 */
		const xml_attribute_iterator& operator--();
		
		/**
		 * Post-decrement operator
		 *
		 * \return old value
		 */
		xml_attribute_iterator operator--(int);
	};

	/**
	 * Abstract tree walker class
	 * \see xml_node::traverse
	 */
	class xml_tree_walker
	{
		friend class xml_node;

	private:
		int _depth;
	
	protected:
		/**
		 * Get node depth
		 * 
		 * \return node depth
		 */
		int depth() const;
	
	public:
		/**
		 * Default ctor
		 */
		xml_tree_walker();

		/**
		 * Virtual dtor
		 */
		virtual ~xml_tree_walker();

	public:
		/**
		 * Callback that is called when traversal of node begins.
		 *
		 * \return returning false will abort the traversal
		 */
		virtual bool begin(xml_node&);

		/**
		 * Callback that is called for each node traversed
		 *
		 * \return returning false will abort the traversal
		 */
		virtual bool for_each(xml_node&) = 0;

		/**
		 * Callback that is called when traversal of node ends.
		 *
		 * \return returning false will abort the traversal
		 */
		virtual bool end(xml_node&);
	};

	/// \internal Memory block
	struct xml_memory_block
	{
		xml_memory_block();

		xml_memory_block* next;
		size_t size;

		char data[memory_block_size];
	};

	/**
	 * Struct used to distinguish parsing with ownership transfer from parsing without it.
	 * \see xml_document::parse
	 */
	struct transfer_ownership_tag {};

	/**
	 * Document class (DOM tree root).
	 * This class has noncopyable semantics (private copy ctor/assignment operator).
	 */
	class xml_document: public xml_node
	{
	private:
		char*				_buffer;

		xml_memory_block	_memory;
		
		xml_document(const xml_document&);
		const xml_document& operator=(const xml_document&);

		void create();
		void free();

	public:
		/**
		 * Default ctor, makes empty document
		 */
		xml_document();

		/**
		 * Dtor
		 */
		~xml_document();

	public:
#ifndef PUGIXML_NO_STL
		/**
		 * Load document from stream.
		 *
		 * \param stream - stream with xml data
		 * \param options - parsing options
		 * \return success flag
		 */
		bool load(std::istream& stream, unsigned int options = parse_default);
#endif

		/**
		 * Load document from string.
		 *
		 * \param contents - input string
		 * \param options - parsing options
		 * \return success flag
		 */
		bool load(const char* contents, unsigned int options = parse_default);

		/**
		 * Load document from file
		 *
		 * \param name - file name
		 * \param options - parsing options
		 * \return success flag
		 */
		bool load_file(const char* name, unsigned int options = parse_default);

		/**
		 * Parse the given XML string in-situ.
		 * The string is modified; you should ensure that string data will persist throughout the
		 * document's lifetime. Although, document does not gain ownership over the string, so you
		 * should free the memory occupied by it manually.
		 *
		 * \param xmlstr - readwrite string with xml data
		 * \param options - parsing options
		 * \return success flag
		 */
		bool parse(char* xmlstr, unsigned int options = parse_default);
		
		/**
		 * Parse the given XML string in-situ (gains ownership).
		 * The string is modified; document gains ownership over the string, so you don't have to worry
		 * about it's lifetime.
		 * Call example: doc.parse(transfer_ownership_tag(), string, options);
		 *
		 * \param xmlstr - readwrite string with xml data
		 * \param options - parsing options
		 * \return success flag
		 */
		bool parse(const transfer_ownership_tag&, char* xmlstr, unsigned int options = parse_default);
		
#ifndef PUGIXML_NO_STL
		/**
		 * Save XML to file
		 *
		 * \param name - file name
		 * \param indent - indentation string
		 * \param flags - formatting flags
		 * \return success flag
		 */
		bool save_file(const char* name, const char* indent = "\t", unsigned int flags = format_default);
#endif

		/**
		 * Compute document order for the whole tree
		 * Sometimes this makes evaluation of XPath queries faster.
		 */
		void precompute_document_order();
	};

#ifndef PUGIXML_NO_XPATH
	/**
	 * XPath exception class.
	 */
	class xpath_exception: public std::exception
	{
	private:
		const char* m_message;

	public:
		/**
		 * Construct exception from static error string
		 *
		 * \param message - error string
		 */
		explicit xpath_exception(const char* message);

		/**
		 * Return error message
		 *
		 * \return error message
		 */
		virtual const char* what() const throw();
	};
	
	/**
	 * XPath node class.
	 * 
	 * XPath defines node to be either xml_node or xml_attribute in pugixml terminology, so xpath_node
	 * is either xml_node or xml_attribute.
	 */
	class xpath_node
	{
	private:
		xml_node m_node;
		xml_attribute m_attribute;
	
    	/// \internal Safe bool type
    	typedef xml_node xpath_node::*unspecified_bool_type;

	public:
		/**
		 * Construct empty XPath node
		 */
		xpath_node();
		
		/**
		 * Construct XPath node from XML node
		 *
		 * \param node - XML node
		 */
		xpath_node(const xml_node& node);

		/**
		 * Construct XPath node from XML attribute
		 *
		 * \param attribute - XML attribute
		 * \param parent - attribute's parent node
		 */
		xpath_node(const xml_attribute& attribute, const xml_node& parent);

		/**
		 * Get XML node, if any
		 *
		 * \return contained XML node, empty node otherwise
		 */
		xml_node node() const;
		
		/**
		 * Get XML attribute, if any
		 *
		 * \return contained XML attribute, if any, empty attribute otherwise
		 */
		xml_attribute attribute() const;
		
		/**
		 * Get parent of contained XML attribute, if any
		 *
		 * \return parent of contained XML attribute, if any, empty node otherwise
		 */
		xml_node parent() const;

    	/**
    	 * Safe bool conversion.
    	 * Allows xpath_node to be used in a context where boolean variable is expected, such as 'if (node)'.
    	 */
		operator unspecified_bool_type() const;
		
		/**
		 * Compares two XPath nodes
		 *
		 * \param n - XPath node to compare to
		 * \return comparison result
		 */
		bool operator==(const xpath_node& n) const;
		
		/**
		 * Compares two XPath nodes
		 *
		 * \param n - XPath node to compare to
		 * \return comparison result
		 */
		bool operator!=(const xpath_node& n) const;
	};

	/**
	 * Not necessarily ordered constant collection of XPath nodes
	 */
	class xpath_node_set
	{
		friend class xpath_ast_node;
		
	public:
		/// Collection type
		enum type_t
		{
			type_unsorted,			///< Not ordered
			type_sorted,			///< Sorted by document order (ascending)
			type_sorted_reverse		///< Sorted by document order (descending)
		};
		
		/// Constant iterator type
		typedef const xpath_node* const_iterator;
	
	private:
		type_t m_type;
		
		xpath_node m_storage;
		
		xpath_node* m_begin;
		xpath_node* m_end;
		xpath_node* m_eos;
		
		bool m_using_storage;
		
		typedef xpath_node* iterator;

		iterator mut_begin();
		iterator mut_end();
		
		void push_back(const xpath_node& n);
		
		template <typename Iterator> void append(Iterator begin, Iterator end);
		
		void truncate(iterator it);

		void remove_duplicates();

	public:
		/**
		 * Default ctor
		 * Constructs empty set
		 */
		xpath_node_set();

		/**
         * Dtor
         */
		~xpath_node_set();
		
		/**
		 * Copy ctor
		 *
		 * \param ns - set to copy
		 */
		xpath_node_set(const xpath_node_set& ns);

		/**
		 * Assignment operator
		 *
		 * \param ns - set to assign
		 * \return self
		 */
		xpath_node_set& operator=(const xpath_node_set& ns);
		
		/**
		 * Get collection type
		 *
		 * \return collection type
		 */
		type_t type() const;
		
		/**
		 * Get collection size
		 *
		 * \return collection size
		 */
		size_t size() const;
		
		/**
		 * Get begin constant iterator for collection
		 *
		 * \return begin constant iterator
		 */
		const_iterator begin() const;
		
		/**
		 * Get end iterator for collection
		 *
		 * \return end iterator
		 */
		const_iterator end() const;
		
		/**
		 * Sort the collection in ascending/descending order by document order
		 *
		 * \param reverse - whether to sort in ascending (false) or descending (true) order
		 */
		void sort(bool reverse = false);
		
		/**
		 * Get first node in the collection by document order
		 *
		 * \return first node by document order
		 */
		xpath_node first() const;
		
		/**
		 * Return true if collection is empty
		 *
		 * \return true if collection is empty, false otherwise
		 */
		bool empty() const;
	};
#endif

#ifndef PUGIXML_NO_STL
	/**
	 * Convert utf16 to utf8
	 *
	 * \param str - input UTF16 string
	 * \return output UTF8 string
	 */
	std::string as_utf8(const wchar_t* str);
	
	/**
	 * Convert utf8 to utf16
	 *
	 * \param str - input UTF8 string
	 * \return output UTF16 string
	 */
	std::wstring as_utf16(const char* str);
#endif
}

// Inline implementation

namespace pugi
{
	namespace impl
	{
		int strcmpwild(const char*, const char*);
	}

	template <typename OutputIterator> void xml_node::all_elements_by_name(const char* name, OutputIterator it) const
	{
		if (empty()) return;
		
		for (xml_node node = first_child(); node; node = node.next_sibling())
		{
			if (node.type() == node_element)
			{
				if (!strcmp(name, node.name()))
				{
					*it = node;
					++it;
				}
			
				if (node.first_child()) node.all_elements_by_name(name, it);
			}
		}
	}

	template <typename OutputIterator> void xml_node::all_elements_by_name_w(const char* name, OutputIterator it) const
	{
		if (empty()) return;
		
		for (xml_node node = first_child(); node; node = node.next_sibling())
		{
			if (node.type() == node_element)
			{
				if (!impl::strcmpwild(name, node.name()))
				{
					*it = node;
					++it;
				}
					
				if (node.first_child()) node.all_elements_by_name_w(name, it);
			}
		}
	}
	
	template <typename Predicate> inline xml_attribute xml_node::find_attribute(Predicate pred) const
	{
		if (!empty())
			for (xml_attribute attrib = first_attribute(); attrib; attrib = attrib.next_attribute())
				if (pred(attrib))
					return attrib;
		
		return xml_attribute();
	}

	template <typename Predicate> inline xml_node xml_node::find_child(Predicate pred) const
	{
		if (!empty())
			for (xml_node node = first_child(); node; node = node.next_sibling())
				if (pred(node))
					return node;

		return xml_node();
	}

	template <typename Predicate> inline xml_node xml_node::find_node(Predicate pred) const
	{
		if (!empty())
			for (xml_node node = first_child(); node; node = node.next_sibling())
			{
				if (pred(node))
					return node;
				
				if (node.first_child())
				{
					xml_node found = node.find_node(pred);
					if (found) return found;
				}
			}

		return xml_node();
	}
}

#endif
