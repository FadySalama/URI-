#pragma once
#include <iostream>
#include <map>
#include <ostream>
#include <regex>
#include <stdexcept>
#include <string>

namespace uripp {
    /**
     * @brief A class representing an absolute or relative Uri and its components
     * 
     */
    class Uri {
        public:
        /**
        * @brief An enum for the different components in an absolute URI
        * 
        */
        enum class absolute_uri_components {
            scheme,
            authority,
            path,
            query,
            fragment
        };

        /**
         * @brief returns string representation of the corresponding absolute URI component
         * 
         * @param comp absolute_uri_components representation
         * @return std::string one of "scheme", authority", "path", "query", "fragment"
         */
        static std::string ComponentToString(absolute_uri_components comp){
            std::string strings[] = {"scheme", "authority", "path", "query", "fragment"};
            return strings[int(comp)];
        }

        /**
        * @brief An enum for the different components in a relative URI
        * 
        */
        enum class relative_uri_components {
            path,
            query,
            fragment
        };

        /**
         * @brief returns string representation of the corresponding relative URI component
         * 
         * @param comp relative_uri_components representation
         * @return std::string one of "path", "query", "fragment"
         */
        static std::string ComponentToString(relative_uri_components comp){
            std::string strings[] = {"path", "query", "fragment"};
            return strings[int(comp)];
        }

        /**
        * @brief An enum for the different components of the Authority URI component
        * 
        */
        enum class authority_components {
            username,
            password,
            host,
            port
        };

        /**
         * @brief returns string representation of the corresponding Authority component
         * 
         * @param comp authority_components representation
         * @return std::string one of "username", "password", "host", "port"
         */
        static std::string ComponentToString(authority_components comp){
            std::string strings[] = {"username", "password", "host", "port"};
            return strings[int(comp)];
        }

        /**
         * @brief Construct a new Uri object
         * 
         * @param href URI string
         * @throws std::invalid_argument if cannot parse URI or Authority component of absolute URI
         */
        Uri(std::string href) {
            this->href = href;

            std::smatch uri_groups;
            is_absolute_uri = std::regex_match(href, uri_groups, uri_regex);
            std::string schemeMatch = uri_groups[2].str();

            if(is_absolute_uri && !schemeMatch.empty()) {
                uri_components_map[absolute_uri_components::scheme] = schemeMatch;

                std::string authorityMatch = uri_groups[4].str();
                if (!authorityMatch.empty()) {
                    uri_components_map[absolute_uri_components::authority] = uri_groups[4].str();
                    has_authority = true; 
                }

                std::string pathMatch = uri_groups[5].str();
                if (!pathMatch.empty()) {
                    uri_components_map[absolute_uri_components::path] = pathMatch;
                }

                std::string queryMatch = uri_groups[7].str();
                if (!queryMatch.empty()) {
                    uri_components_map[absolute_uri_components::query] = queryMatch;
                    has_query = true;
                }

                std::string fragmentMatch = uri_groups[9].str();
                if (!fragmentMatch.empty()) {
                    uri_components_map[absolute_uri_components::fragment] = fragmentMatch;
                    has_fragment = true;
                }

                if(has_authority) {
                    std::smatch authority_groups;
                    bool isValidAuthority =  std::regex_match(uri_components_map[absolute_uri_components::authority], authority_groups, authority_regex);

                    if(!isValidAuthority) {
                        throw std::invalid_argument("The authority component of href is not valid");
                    }

                    std::string userMatch = authority_groups[2].str();
                    if (!userMatch.empty()) {
                        authority_components_map[authority_components::username] = userMatch;
                        has_username = true;
                    }
                    
                    std::string passMatch = authority_groups[4].str();
                    if (!passMatch.empty()) {
                        authority_components_map[authority_components::password] = passMatch;
                        has_password = true;
                    }

                    std::string hostMatch = authority_groups[5].str();
                    authority_components_map[authority_components::host] = hostMatch;
                    if(authority_components_map[authority_components::host][0] == '[') isIPv6Host = true;

                    std::string portMatch = authority_groups[7].str();
                    if (!portMatch.empty()) {
                        authority_components_map[authority_components::port] = portMatch;
                        has_port = true;
                    }
                }
            } else {
                is_absolute_uri = false;
                is_relative_uri = std::regex_match(href, uri_groups, relative_uri_regex);

                if(!is_relative_uri) {
                    throw std::invalid_argument("The provided href is not a valid absolute or relative URI");
                }   

                std::string pathMatch = uri_groups[1].str();
                if (!pathMatch.empty()) {
                    relative_uri_components_map[relative_uri_components::path] = pathMatch;
                }

                std::string queryMatch = uri_groups[2].str();
                if (!queryMatch.empty()) {
                    relative_uri_components_map[relative_uri_components::query] = queryMatch;
                    has_query = true;
                }

                std::string fragmentMatch = uri_groups[3].str();
                if (!fragmentMatch.empty()) {
                    relative_uri_components_map[relative_uri_components::fragment] = fragmentMatch;
                    has_fragment = true;
                }
            }
        }

        Uri(const Uri& other) {
            Uri(other.href);
        }

        /**
         * @brief Get the Href string
         * 
         * @return const std::string
         */
        const std::string getHref() {
            return href;
        }

        /**
         * @brief Get the Scheme component of absolute URI
         * 
         * @return const std::string Scheme component
         * @throws std::domain_error if a relative URI
         */
        const std::string getScheme() {
            if(is_relative_uri) throw std::domain_error("Cannot use with relative URI");
            return uri_components_map[absolute_uri_components::scheme];

        }

        /**
         * @brief Get the Authority component of absolute URI
         * 
         * @return const std::string Authority component
         * @throws std::domain_error if a relative URI
         */
        const std::string getAuthority() {
            if(has_authority) 
                return uri_components_map[absolute_uri_components::authority];
            else 
                throw std::domain_error("Cannot get authority of a relative URI");
        }

        /**
         * @brief Get the Path object
         * 
         * @return const std::string 
         */
        const std::string getPath() {
            if (is_absolute_uri) {
                return uri_components_map[absolute_uri_components::path];
            } else if (is_relative_uri) {
                return relative_uri_components_map[relative_uri_components::path];
            }

            return "";
        }

        /**
         * @brief Get the Query component of the absolute or relative URI
         * 
         * @return const std::string the Query component if available; otherwise an empty string
         */
        const std::string getQuery() {
            if (is_absolute_uri && has_query) {
                return uri_components_map[absolute_uri_components::query];
            } else if (is_relative_uri && has_query) {
                return relative_uri_components_map[relative_uri_components::query];
            } else {
                return "";
            }
        }

        /**
         * @brief Get the Fragment component of the absolute or relative URI 
         * 
         * @return const std::string the Fragment component if available; otherwise an empty string
         */
        const std::string getFragment() {
            if (is_absolute_uri && has_fragment) {
                return uri_components_map[absolute_uri_components::fragment];
            } else if (is_relative_uri && has_fragment) {
                return relative_uri_components_map[relative_uri_components::fragment];
            } else {
                return "";
            }
        }

        /**
         * @brief Get a map containing all components of the absolute URI
         * 
         * @return std::map<absolute_uri_components, std::string>* a mapping from component ID to its string
         * @throws std::domain_error if URI is relative
         */
        std::map<absolute_uri_components, std::string>* getURIComponents() {
            if(is_absolute_uri) {
                return &uri_components_map;
            }
            
            throw std::domain_error("Cannot use with relative URI");
        }

        /**
         * @brief  Get a map containing all components of the relative URI
         * 
         * @return std::map<relative_uri_components, std::string>*  a mapping from component ID to its string
         * @throws std::domain_error if URI is absolute
         */
        std::map<relative_uri_components, std::string>* getRelativeURIComponents() {
            if(is_relative_uri) {
                return &relative_uri_components_map;
            } else {
                throw std::domain_error("Cannot use with absolute URI");
            };
        }

        /**
         * @brief Get the Username component of the Authority component
         * 
         * @return const std::string Username component if available; otherwise an empty string
         * @throws std::domain_error if URI is relative
         */
        const std::string getUsername() {
            if(is_relative_uri) throw std::domain_error("Cannot use with relative URI");

            if(has_username) 
                return authority_components_map[authority_components::username];
            else 
                return "";
        }

        /**
         * @brief Get the Password component of the Authority component
         * 
         * @return const std::string Password component if available; otherwise an empty string
         * @throws std::domain_error if URI is relative
         */
        const std::string getPassword() {
            if(is_relative_uri) throw std::domain_error("Cannot use with relative URI");

            if(has_password) 
                return authority_components_map[authority_components::password];
            else 
                return "";
        }

        /**
         * @brief Get the Host component of the Authority component
         * 
         * @return const std::string Host component if available; otherwise an empty string
         * @throws std::domain_error if URI is relative
         */
        const std::string getHost() {
            if(is_relative_uri) throw std::domain_error("Cannot use with relative URI");

            if(has_authority) 
                return authority_components_map[authority_components::host];
            else 
                return "";
        }

        /**
         * @brief Get the Port component of the Authority component
         * 
         * @return const std::string Port component if available; otherwise an empty string
         * @throws std::domain_error if URI is relative
         */
        const std::string getPort() {
            if(is_relative_uri) throw std::domain_error("Cannot use with relative URI");

            if(has_port) 
                return authority_components_map[authority_components::port];
            else 
                return "";
        }

        /**
         * @brief  Get a map containing all components of the relative URI
         * 
         * @return std::map<relative_uri_components, std::string>*  a mapping from component ID to its string if available; otherwise a nullptr
         * @throws std::domain_error if relative URI
         */
        std::map<authority_components, std::string>* getAuthorityComponents() {
            if(is_relative_uri) throw std::domain_error("Cannot use with relative URI");

            if(is_absolute_uri && has_authority) {
                return &authority_components_map;
            }
            else {
                return nullptr;
            }    
        }

        /**
         * @brief Indicates if absolute URI has a Authority component
         * 
         * @return true if the absolute URI has Authority
         * @return false if the absolute URI does not have Authority
         */
        bool hasAuthority() {
            return has_authority;
        }

        /**
         * @brief Indicates if the relative or absolute URI has a Query component
         * 
         * @return true if the relative or absolute URI has Query 
         * @return false if the relative or absolute URI does not have Query
         */
        bool hasQuery() {
            return has_query;
        }


        /**
         * @brief Indicates if the relative or absolute URI has a Fragment component
         * 
         * @return true if the relative or absolute URI has Fragment 
         * @return false if the relative or absolute URI does not have Fragment
         */
        bool hasFragment() {
            return has_fragment;
        }
        
        /**
         * @brief Indicates if the URI has a username in the Authority component
         * 
         * @return true if Authority component contains username
         * @return false if Authority component does not contains username
         */
        bool hasUsername() {
            return has_username;
        }

        /**
         * @brief Indicates if the URI has a password in the Authority component
         * 
         * @return true if Authority component contains password
         * @return false if Authority component does not contains password
         */
        bool hasPassword() {
            return has_password;
        }

        /**
         * @brief Indicates if the URI has a port in the Authority component 
         * 
         * @return true if Authority component contains port
         * @return false if Authority component does not contains port
         */
        bool hasPort() {
            return has_port;
        }

        /**
         * @brief Indicates if the URI is relative
         * 
         * @return true if relative URI
         * @return false if absolute URI
         */
        bool isRelativeUri() {
            return is_relative_uri;
        }

        /**
         * @brief << Operator overload
         * 
         * @param os output stream
         * @param uri Uri object
         * @return std::ostream& output stream with added href
         */
        friend std::ostream& operator<<(std::ostream& os, const Uri& uri) { 
            os << uri.href;
            return os;
        }

        private:
        Uri() {

        }

        std::string href = "";

        std::regex uri_regex = std::regex("^(([^:/?#]+):)?(\\/\\/([^/?#]*))?([^?#]*)(\\?([^#]*))?(#(.*))?", std::regex_constants::ECMAScript);

        std::regex authority_regex = std::regex("^(([^@]+)(:([^@]+))@)?(\\[[^\\]]+\\]|[^:\\[\\]]+)(:(\\d+))?$", std::regex_constants::ECMAScript);

        std::regex relative_uri_regex = std::regex("^((?:\\/\\/(?:[A-Za-z0-9\\-._~%!$&'()*+,;=:@]*)(?:\\/(?:[A-Za-z0-9\\-._~%!$&'()*+,;=:@]*))*)|(?:\\/(?:[A-Za-z0-9\\-._~%!$&'()*+,;=:@]+(?:\\/(?:[A-Za-z0-9\\-._~%!$&'()*+,;=:@]*))*))|(?:[A-Za-z0-9\\-._~%!$&'()*+,;=@]+(?:\\/(?:[A-Za-z0-9\\-._~%!$&'()*+,;=:@]*))*))?(\\?[A-Za-z0-9\\-._~%!$&'()*+,;=:@\\/\\?]*)?(\\#[A-Za-z0-9\\-._~%!$&'()*+,;=:@\\/\\?]*)?$");

        std::map<absolute_uri_components, std::string> uri_components_map = {{absolute_uri_components::scheme, ""}, {absolute_uri_components::path, ""}};
        std::map<authority_components, std::string> authority_components_map;
        std::map<relative_uri_components, std::string> relative_uri_components_map;

        bool is_absolute_uri = false;
        bool is_relative_uri = false;

        bool has_authority = false;
        bool has_query = false;
        bool has_fragment = false;

        bool isIPv6Host = false;
        bool has_username = false;
        bool has_password = false;
        bool has_port = false;
    };

    /**
     * @brief A class for specifying all configurations for building a URI
     * 
     */
    class UriBuilderConfig {
        public:
        enum class config_uri_parameters {
            scheme,
            authority,
            path,
            query,
            fragment,
            username,
            password,
            host,
            port
        };

        /**
         * @brief Construct a new UriBuilderConfig object
         * 
         */
        UriBuilderConfig() {};

        /**
         * @brief Construct a new Uri Builder Config object using mapping
         * 
         * @param configMap 
         * @param is_hierarchical indicates if a Uri is hierarchical or not; default true
         * @param non_hierarchical_delimiter specifies non-hierarchical path delimiter; default ':'
         * @param query_delimiter specifies query delimiter; default ','
         */
        UriBuilderConfig(std::map<config_uri_parameters, std::string> configMap, bool is_hierarchical = true, char non_hierarchical_delimiter = ':', char query_delimiter = ',') {
            scheme = configMap[config_uri_parameters::scheme];
            authority = configMap[config_uri_parameters::authority];
            path = configMap[config_uri_parameters::path];
            query = configMap[config_uri_parameters::query];
            fragment = configMap[config_uri_parameters::fragment];

            username = configMap[config_uri_parameters::username];
            password = configMap[config_uri_parameters::password];
            host = configMap[config_uri_parameters::host];
            port = configMap[config_uri_parameters::port];

            this->is_hierarchical = is_hierarchical;
            this->non_hierarchical_delimiter = non_hierarchical_delimiter;
            this->query_delimiter = query_delimiter;
        }

        std::string scheme = "";
        std::string authority = "";
        std::string path = "";
        std::string query = "";
        std::string fragment = "";

        std::string username = "";
        std::string password = "";
        std::string host = "";
        std::string port = "";

        bool is_hierarchical = true;
        char non_hierarchical_delimiter = ':';
        char query_delimiter = ',';
    };

    /**
     * @brief A class for building a URI
     * 
     */
    class UriBuilder {
        public:

        /**
         * @brief builds a Uri object using a UriBuilderConfig and return a pointer to it
         * 
         * @param config configuration of Uri
         * @return Uri* pointer to Uri object
         */
        static Uri* build(UriBuilderConfig config) {
            std::string href = "";

            if(!config.scheme.empty()) {
                href += config.scheme;
                href += config.is_hierarchical ? "://" : ":";

                if(!config.authority.empty()) {
                    href += config.authority;
                } else {
                    // Construct Authority out of components
                    std::string authority = "";
                    authority += config.username;
                    authority += authority.empty() ? ":"+config.password : ""; // add password only if username is available
                    authority += authority.empty() ? "@" : "";
                    authority += config.host;
                    authority += ":"+config.port;
                    href += authority;
                }
            }

            if(!href.empty()) {
                bool starts_with_delimiter = config.is_hierarchical ? config.path[0] == '/' : config.path[0] == config.non_hierarchical_delimiter;
                href += config.is_hierarchical && !starts_with_delimiter && href[href.size()-1] != '/' ?  "/" : "";
            }

            href += config.path;
            href += "?"+config.query;
            href += "#"+config.fragment;

            std::cout << "Building URI for href: " << href << std::endl; 

            return new Uri(href);
        }

        /**
         * @brief builds a Uri object using a configuration map and return a pointer to it
         * 
         * @param configMap configuration map
         * @param is_hierarchical indicates if a Uri is hierarchical or not; default true
         * @param non_hierarchical_delimiter specifies non-hierarchical path delimiter; default ':'
         * @param query_delimiter specifies query delimiter; default ','
         * @return Uri* 
         */
        static Uri* build(std::map<UriBuilderConfig::config_uri_parameters, std::string> configMap, bool is_hierarchical = true, char non_hierarchical_delimiter = ':', char query_delimiter = ',') {
            UriBuilderConfig config = UriBuilderConfig(configMap, is_hierarchical, non_hierarchical_delimiter, query_delimiter);
            return build(config);
        }
    };
}
