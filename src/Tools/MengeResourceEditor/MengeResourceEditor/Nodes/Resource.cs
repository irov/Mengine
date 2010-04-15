using System;
using System.Collections.Generic;
using System.Text;

namespace MengeResourceEditor.Nodes
{
    class Resource: Node
    {
        protected string name;
        protected string type;
        /// <summary>
        /// 
        /// </summary>
        /// <param name="_name"></param>
        /// <param name="_type"></param>
        public Resource(string _name, string _type)
        {
            name = _name;
            type = _type;
        }

        public Resource()
        {

        }
        /// <summary>
        /// 
        /// </summary>
        public string Name
        {
            get
            {
                return name;
            }
            set
            {
                name = value;
            }
        }
        /// <summary>
        /// 
        /// </summary>
        public string Type
        {
            get
            {
                return type;
            }
        }
        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        public override string ToString ()
        {
            return Name;
        }
    }
}
