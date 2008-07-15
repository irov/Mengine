using System;
using System.Collections.Generic;
using System.Text;

namespace Editor
{
    class ResourceManager
    {
        public void AddResource(ResourceImageDefault resource)
        {
            resources.Add(resource.GetName(), resource);
        }

        public ResourceImageDefault GetResource(String name)
        {
            ResourceImageDefault result = null;
            resources.TryGetValue(name, out result);
            return result;
        }

        public Dictionary<String, ResourceImageDefault>.ValueCollection GetValues()
        {
            return resources.Values;
        }

        private Dictionary<String, ResourceImageDefault> resources = new Dictionary<String, ResourceImageDefault>();
    }
}