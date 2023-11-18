/*eslint-disable block-scoped-var, id-length, no-control-regex, no-magic-numbers, no-prototype-builtins, no-redeclare, no-shadow, no-var, sort-vars*/
"use strict";

var $protobuf = require("protobufjs/minimal");

// Common aliases
var $Reader = $protobuf.Reader, $Writer = $protobuf.Writer, $util = $protobuf.util;

// Exported root namespace
var $root = $protobuf.roots["default"] || ($protobuf.roots["default"] = {});

$root.nam = (function() {

    /**
     * Namespace nam.
     * @exports nam
     * @namespace
     */
    var nam = {};

    nam.LoginRequest = (function() {

        /**
         * Properties of a LoginRequest.
         * @memberof nam
         * @interface ILoginRequest
         * @property {Uint8Array|null} [messagetype] LoginRequest messagetype
         * @property {string|null} [username] LoginRequest username
         * @property {string|null} [password] LoginRequest password
         */

        /**
         * Constructs a new LoginRequest.
         * @memberof nam
         * @classdesc Represents a LoginRequest.
         * @implements ILoginRequest
         * @constructor
         * @param {nam.ILoginRequest=} [properties] Properties to set
         */
        function LoginRequest(properties) {
            if (properties)
                for (var keys = Object.keys(properties), i = 0; i < keys.length; ++i)
                    if (properties[keys[i]] != null)
                        this[keys[i]] = properties[keys[i]];
        }

        /**
         * LoginRequest messagetype.
         * @member {Uint8Array} messagetype
         * @memberof nam.LoginRequest
         * @instance
         */
        LoginRequest.prototype.messagetype = $util.newBuffer([]);

        /**
         * LoginRequest username.
         * @member {string} username
         * @memberof nam.LoginRequest
         * @instance
         */
        LoginRequest.prototype.username = "";

        /**
         * LoginRequest password.
         * @member {string} password
         * @memberof nam.LoginRequest
         * @instance
         */
        LoginRequest.prototype.password = "";

        /**
         * Creates a new LoginRequest instance using the specified properties.
         * @function create
         * @memberof nam.LoginRequest
         * @static
         * @param {nam.ILoginRequest=} [properties] Properties to set
         * @returns {nam.LoginRequest} LoginRequest instance
         */
        LoginRequest.create = function create(properties) {
            return new LoginRequest(properties);
        };

        /**
         * Encodes the specified LoginRequest message. Does not implicitly {@link nam.LoginRequest.verify|verify} messages.
         * @function encode
         * @memberof nam.LoginRequest
         * @static
         * @param {nam.ILoginRequest} message LoginRequest message or plain object to encode
         * @param {$protobuf.Writer} [writer] Writer to encode to
         * @returns {$protobuf.Writer} Writer
         */
        LoginRequest.encode = function encode(message, writer) {
            if (!writer)
                writer = $Writer.create();
            if (message.messagetype != null && Object.hasOwnProperty.call(message, "messagetype"))
                writer.uint32(/* id 1, wireType 2 =*/10).bytes(message.messagetype);
            if (message.username != null && Object.hasOwnProperty.call(message, "username"))
                writer.uint32(/* id 2, wireType 2 =*/18).string(message.username);
            if (message.password != null && Object.hasOwnProperty.call(message, "password"))
                writer.uint32(/* id 3, wireType 2 =*/26).string(message.password);
            return writer;
        };

        /**
         * Encodes the specified LoginRequest message, length delimited. Does not implicitly {@link nam.LoginRequest.verify|verify} messages.
         * @function encodeDelimited
         * @memberof nam.LoginRequest
         * @static
         * @param {nam.ILoginRequest} message LoginRequest message or plain object to encode
         * @param {$protobuf.Writer} [writer] Writer to encode to
         * @returns {$protobuf.Writer} Writer
         */
        LoginRequest.encodeDelimited = function encodeDelimited(message, writer) {
            return this.encode(message, writer).ldelim();
        };

        /**
         * Decodes a LoginRequest message from the specified reader or buffer.
         * @function decode
         * @memberof nam.LoginRequest
         * @static
         * @param {$protobuf.Reader|Uint8Array} reader Reader or buffer to decode from
         * @param {number} [length] Message length if known beforehand
         * @returns {nam.LoginRequest} LoginRequest
         * @throws {Error} If the payload is not a reader or valid buffer
         * @throws {$protobuf.util.ProtocolError} If required fields are missing
         */
        LoginRequest.decode = function decode(reader, length) {
            if (!(reader instanceof $Reader))
                reader = $Reader.create(reader);
            var end = length === undefined ? reader.len : reader.pos + length, message = new $root.nam.LoginRequest();
            while (reader.pos < end) {
                var tag = reader.uint32();
                switch (tag >>> 3) {
                case 1:
                    message.messagetype = reader.bytes();
                    break;
                case 2:
                    message.username = reader.string();
                    break;
                case 3:
                    message.password = reader.string();
                    break;
                default:
                    reader.skipType(tag & 7);
                    break;
                }
            }
            return message;
        };

        /**
         * Decodes a LoginRequest message from the specified reader or buffer, length delimited.
         * @function decodeDelimited
         * @memberof nam.LoginRequest
         * @static
         * @param {$protobuf.Reader|Uint8Array} reader Reader or buffer to decode from
         * @returns {nam.LoginRequest} LoginRequest
         * @throws {Error} If the payload is not a reader or valid buffer
         * @throws {$protobuf.util.ProtocolError} If required fields are missing
         */
        LoginRequest.decodeDelimited = function decodeDelimited(reader) {
            if (!(reader instanceof $Reader))
                reader = new $Reader(reader);
            return this.decode(reader, reader.uint32());
        };

        /**
         * Verifies a LoginRequest message.
         * @function verify
         * @memberof nam.LoginRequest
         * @static
         * @param {Object.<string,*>} message Plain object to verify
         * @returns {string|null} `null` if valid, otherwise the reason why it is not
         */
        LoginRequest.verify = function verify(message) {
            if (typeof message !== "object" || message === null)
                return "object expected";
            if (message.messagetype != null && message.hasOwnProperty("messagetype"))
                if (!(message.messagetype && typeof message.messagetype.length === "number" || $util.isString(message.messagetype)))
                    return "messagetype: buffer expected";
            if (message.username != null && message.hasOwnProperty("username"))
                if (!$util.isString(message.username))
                    return "username: string expected";
            if (message.password != null && message.hasOwnProperty("password"))
                if (!$util.isString(message.password))
                    return "password: string expected";
            return null;
        };

        /**
         * Creates a LoginRequest message from a plain object. Also converts values to their respective internal types.
         * @function fromObject
         * @memberof nam.LoginRequest
         * @static
         * @param {Object.<string,*>} object Plain object
         * @returns {nam.LoginRequest} LoginRequest
         */
        LoginRequest.fromObject = function fromObject(object) {
            if (object instanceof $root.nam.LoginRequest)
                return object;
            var message = new $root.nam.LoginRequest();
            if (object.messagetype != null)
                if (typeof object.messagetype === "string")
                    $util.base64.decode(object.messagetype, message.messagetype = $util.newBuffer($util.base64.length(object.messagetype)), 0);
                else if (object.messagetype.length)
                    message.messagetype = object.messagetype;
            if (object.username != null)
                message.username = String(object.username);
            if (object.password != null)
                message.password = String(object.password);
            return message;
        };

        /**
         * Creates a plain object from a LoginRequest message. Also converts values to other types if specified.
         * @function toObject
         * @memberof nam.LoginRequest
         * @static
         * @param {nam.LoginRequest} message LoginRequest
         * @param {$protobuf.IConversionOptions} [options] Conversion options
         * @returns {Object.<string,*>} Plain object
         */
        LoginRequest.toObject = function toObject(message, options) {
            if (!options)
                options = {};
            var object = {};
            if (options.defaults) {
                if (options.bytes === String)
                    object.messagetype = "";
                else {
                    object.messagetype = [];
                    if (options.bytes !== Array)
                        object.messagetype = $util.newBuffer(object.messagetype);
                }
                object.username = "";
                object.password = "";
            }
            if (message.messagetype != null && message.hasOwnProperty("messagetype"))
                object.messagetype = options.bytes === String ? $util.base64.encode(message.messagetype, 0, message.messagetype.length) : options.bytes === Array ? Array.prototype.slice.call(message.messagetype) : message.messagetype;
            if (message.username != null && message.hasOwnProperty("username"))
                object.username = message.username;
            if (message.password != null && message.hasOwnProperty("password"))
                object.password = message.password;
            return object;
        };

        /**
         * Converts this LoginRequest to JSON.
         * @function toJSON
         * @memberof nam.LoginRequest
         * @instance
         * @returns {Object.<string,*>} JSON object
         */
        LoginRequest.prototype.toJSON = function toJSON() {
            return this.constructor.toObject(this, $protobuf.util.toJSONOptions);
        };

        return LoginRequest;
    })();

    nam.SignupRequest = (function() {

        /**
         * Properties of a SignupRequest.
         * @memberof nam
         * @interface ISignupRequest
         * @property {string|null} [username] SignupRequest username
         * @property {string|null} [password] SignupRequest password
         * @property {string|null} [email] SignupRequest email
         */

        /**
         * Constructs a new SignupRequest.
         * @memberof nam
         * @classdesc Represents a SignupRequest.
         * @implements ISignupRequest
         * @constructor
         * @param {nam.ISignupRequest=} [properties] Properties to set
         */
        function SignupRequest(properties) {
            if (properties)
                for (var keys = Object.keys(properties), i = 0; i < keys.length; ++i)
                    if (properties[keys[i]] != null)
                        this[keys[i]] = properties[keys[i]];
        }

        /**
         * SignupRequest username.
         * @member {string} username
         * @memberof nam.SignupRequest
         * @instance
         */
        SignupRequest.prototype.username = "";

        /**
         * SignupRequest password.
         * @member {string} password
         * @memberof nam.SignupRequest
         * @instance
         */
        SignupRequest.prototype.password = "";

        /**
         * SignupRequest email.
         * @member {string} email
         * @memberof nam.SignupRequest
         * @instance
         */
        SignupRequest.prototype.email = "";

        /**
         * Creates a new SignupRequest instance using the specified properties.
         * @function create
         * @memberof nam.SignupRequest
         * @static
         * @param {nam.ISignupRequest=} [properties] Properties to set
         * @returns {nam.SignupRequest} SignupRequest instance
         */
        SignupRequest.create = function create(properties) {
            return new SignupRequest(properties);
        };

        /**
         * Encodes the specified SignupRequest message. Does not implicitly {@link nam.SignupRequest.verify|verify} messages.
         * @function encode
         * @memberof nam.SignupRequest
         * @static
         * @param {nam.ISignupRequest} message SignupRequest message or plain object to encode
         * @param {$protobuf.Writer} [writer] Writer to encode to
         * @returns {$protobuf.Writer} Writer
         */
        SignupRequest.encode = function encode(message, writer) {
            if (!writer)
                writer = $Writer.create();
            if (message.username != null && Object.hasOwnProperty.call(message, "username"))
                writer.uint32(/* id 1, wireType 2 =*/10).string(message.username);
            if (message.password != null && Object.hasOwnProperty.call(message, "password"))
                writer.uint32(/* id 2, wireType 2 =*/18).string(message.password);
            if (message.email != null && Object.hasOwnProperty.call(message, "email"))
                writer.uint32(/* id 3, wireType 2 =*/26).string(message.email);
            return writer;
        };

        /**
         * Encodes the specified SignupRequest message, length delimited. Does not implicitly {@link nam.SignupRequest.verify|verify} messages.
         * @function encodeDelimited
         * @memberof nam.SignupRequest
         * @static
         * @param {nam.ISignupRequest} message SignupRequest message or plain object to encode
         * @param {$protobuf.Writer} [writer] Writer to encode to
         * @returns {$protobuf.Writer} Writer
         */
        SignupRequest.encodeDelimited = function encodeDelimited(message, writer) {
            return this.encode(message, writer).ldelim();
        };

        /**
         * Decodes a SignupRequest message from the specified reader or buffer.
         * @function decode
         * @memberof nam.SignupRequest
         * @static
         * @param {$protobuf.Reader|Uint8Array} reader Reader or buffer to decode from
         * @param {number} [length] Message length if known beforehand
         * @returns {nam.SignupRequest} SignupRequest
         * @throws {Error} If the payload is not a reader or valid buffer
         * @throws {$protobuf.util.ProtocolError} If required fields are missing
         */
        SignupRequest.decode = function decode(reader, length) {
            if (!(reader instanceof $Reader))
                reader = $Reader.create(reader);
            var end = length === undefined ? reader.len : reader.pos + length, message = new $root.nam.SignupRequest();
            while (reader.pos < end) {
                var tag = reader.uint32();
                switch (tag >>> 3) {
                case 1:
                    message.username = reader.string();
                    break;
                case 2:
                    message.password = reader.string();
                    break;
                case 3:
                    message.email = reader.string();
                    break;
                default:
                    reader.skipType(tag & 7);
                    break;
                }
            }
            return message;
        };

        /**
         * Decodes a SignupRequest message from the specified reader or buffer, length delimited.
         * @function decodeDelimited
         * @memberof nam.SignupRequest
         * @static
         * @param {$protobuf.Reader|Uint8Array} reader Reader or buffer to decode from
         * @returns {nam.SignupRequest} SignupRequest
         * @throws {Error} If the payload is not a reader or valid buffer
         * @throws {$protobuf.util.ProtocolError} If required fields are missing
         */
        SignupRequest.decodeDelimited = function decodeDelimited(reader) {
            if (!(reader instanceof $Reader))
                reader = new $Reader(reader);
            return this.decode(reader, reader.uint32());
        };

        /**
         * Verifies a SignupRequest message.
         * @function verify
         * @memberof nam.SignupRequest
         * @static
         * @param {Object.<string,*>} message Plain object to verify
         * @returns {string|null} `null` if valid, otherwise the reason why it is not
         */
        SignupRequest.verify = function verify(message) {
            if (typeof message !== "object" || message === null)
                return "object expected";
            if (message.username != null && message.hasOwnProperty("username"))
                if (!$util.isString(message.username))
                    return "username: string expected";
            if (message.password != null && message.hasOwnProperty("password"))
                if (!$util.isString(message.password))
                    return "password: string expected";
            if (message.email != null && message.hasOwnProperty("email"))
                if (!$util.isString(message.email))
                    return "email: string expected";
            return null;
        };

        /**
         * Creates a SignupRequest message from a plain object. Also converts values to their respective internal types.
         * @function fromObject
         * @memberof nam.SignupRequest
         * @static
         * @param {Object.<string,*>} object Plain object
         * @returns {nam.SignupRequest} SignupRequest
         */
        SignupRequest.fromObject = function fromObject(object) {
            if (object instanceof $root.nam.SignupRequest)
                return object;
            var message = new $root.nam.SignupRequest();
            if (object.username != null)
                message.username = String(object.username);
            if (object.password != null)
                message.password = String(object.password);
            if (object.email != null)
                message.email = String(object.email);
            return message;
        };

        /**
         * Creates a plain object from a SignupRequest message. Also converts values to other types if specified.
         * @function toObject
         * @memberof nam.SignupRequest
         * @static
         * @param {nam.SignupRequest} message SignupRequest
         * @param {$protobuf.IConversionOptions} [options] Conversion options
         * @returns {Object.<string,*>} Plain object
         */
        SignupRequest.toObject = function toObject(message, options) {
            if (!options)
                options = {};
            var object = {};
            if (options.defaults) {
                object.username = "";
                object.password = "";
                object.email = "";
            }
            if (message.username != null && message.hasOwnProperty("username"))
                object.username = message.username;
            if (message.password != null && message.hasOwnProperty("password"))
                object.password = message.password;
            if (message.email != null && message.hasOwnProperty("email"))
                object.email = message.email;
            return object;
        };

        /**
         * Converts this SignupRequest to JSON.
         * @function toJSON
         * @memberof nam.SignupRequest
         * @instance
         * @returns {Object.<string,*>} JSON object
         */
        SignupRequest.prototype.toJSON = function toJSON() {
            return this.constructor.toObject(this, $protobuf.util.toJSONOptions);
        };

        return SignupRequest;
    })();

    nam.AuthenticationResponse = (function() {

        /**
         * Properties of an AuthenticationResponse.
         * @memberof nam
         * @interface IAuthenticationResponse
         * @property {boolean|null} [success] AuthenticationResponse success
         * @property {string|null} [message] AuthenticationResponse message
         */

        /**
         * Constructs a new AuthenticationResponse.
         * @memberof nam
         * @classdesc Represents an AuthenticationResponse.
         * @implements IAuthenticationResponse
         * @constructor
         * @param {nam.IAuthenticationResponse=} [properties] Properties to set
         */
        function AuthenticationResponse(properties) {
            if (properties)
                for (var keys = Object.keys(properties), i = 0; i < keys.length; ++i)
                    if (properties[keys[i]] != null)
                        this[keys[i]] = properties[keys[i]];
        }

        /**
         * AuthenticationResponse success.
         * @member {boolean} success
         * @memberof nam.AuthenticationResponse
         * @instance
         */
        AuthenticationResponse.prototype.success = false;

        /**
         * AuthenticationResponse message.
         * @member {string} message
         * @memberof nam.AuthenticationResponse
         * @instance
         */
        AuthenticationResponse.prototype.message = "";

        /**
         * Creates a new AuthenticationResponse instance using the specified properties.
         * @function create
         * @memberof nam.AuthenticationResponse
         * @static
         * @param {nam.IAuthenticationResponse=} [properties] Properties to set
         * @returns {nam.AuthenticationResponse} AuthenticationResponse instance
         */
        AuthenticationResponse.create = function create(properties) {
            return new AuthenticationResponse(properties);
        };

        /**
         * Encodes the specified AuthenticationResponse message. Does not implicitly {@link nam.AuthenticationResponse.verify|verify} messages.
         * @function encode
         * @memberof nam.AuthenticationResponse
         * @static
         * @param {nam.IAuthenticationResponse} message AuthenticationResponse message or plain object to encode
         * @param {$protobuf.Writer} [writer] Writer to encode to
         * @returns {$protobuf.Writer} Writer
         */
        AuthenticationResponse.encode = function encode(message, writer) {
            if (!writer)
                writer = $Writer.create();
            if (message.success != null && Object.hasOwnProperty.call(message, "success"))
                writer.uint32(/* id 1, wireType 0 =*/8).bool(message.success);
            if (message.message != null && Object.hasOwnProperty.call(message, "message"))
                writer.uint32(/* id 2, wireType 2 =*/18).string(message.message);
            return writer;
        };

        /**
         * Encodes the specified AuthenticationResponse message, length delimited. Does not implicitly {@link nam.AuthenticationResponse.verify|verify} messages.
         * @function encodeDelimited
         * @memberof nam.AuthenticationResponse
         * @static
         * @param {nam.IAuthenticationResponse} message AuthenticationResponse message or plain object to encode
         * @param {$protobuf.Writer} [writer] Writer to encode to
         * @returns {$protobuf.Writer} Writer
         */
        AuthenticationResponse.encodeDelimited = function encodeDelimited(message, writer) {
            return this.encode(message, writer).ldelim();
        };

        /**
         * Decodes an AuthenticationResponse message from the specified reader or buffer.
         * @function decode
         * @memberof nam.AuthenticationResponse
         * @static
         * @param {$protobuf.Reader|Uint8Array} reader Reader or buffer to decode from
         * @param {number} [length] Message length if known beforehand
         * @returns {nam.AuthenticationResponse} AuthenticationResponse
         * @throws {Error} If the payload is not a reader or valid buffer
         * @throws {$protobuf.util.ProtocolError} If required fields are missing
         */
        AuthenticationResponse.decode = function decode(reader, length) {
            if (!(reader instanceof $Reader))
                reader = $Reader.create(reader);
            var end = length === undefined ? reader.len : reader.pos + length, message = new $root.nam.AuthenticationResponse();
            while (reader.pos < end) {
                var tag = reader.uint32();
                switch (tag >>> 3) {
                case 1:
                    message.success = reader.bool();
                    break;
                case 2:
                    message.message = reader.string();
                    break;
                default:
                    reader.skipType(tag & 7);
                    break;
                }
            }
            return message;
        };

        /**
         * Decodes an AuthenticationResponse message from the specified reader or buffer, length delimited.
         * @function decodeDelimited
         * @memberof nam.AuthenticationResponse
         * @static
         * @param {$protobuf.Reader|Uint8Array} reader Reader or buffer to decode from
         * @returns {nam.AuthenticationResponse} AuthenticationResponse
         * @throws {Error} If the payload is not a reader or valid buffer
         * @throws {$protobuf.util.ProtocolError} If required fields are missing
         */
        AuthenticationResponse.decodeDelimited = function decodeDelimited(reader) {
            if (!(reader instanceof $Reader))
                reader = new $Reader(reader);
            return this.decode(reader, reader.uint32());
        };

        /**
         * Verifies an AuthenticationResponse message.
         * @function verify
         * @memberof nam.AuthenticationResponse
         * @static
         * @param {Object.<string,*>} message Plain object to verify
         * @returns {string|null} `null` if valid, otherwise the reason why it is not
         */
        AuthenticationResponse.verify = function verify(message) {
            if (typeof message !== "object" || message === null)
                return "object expected";
            if (message.success != null && message.hasOwnProperty("success"))
                if (typeof message.success !== "boolean")
                    return "success: boolean expected";
            if (message.message != null && message.hasOwnProperty("message"))
                if (!$util.isString(message.message))
                    return "message: string expected";
            return null;
        };

        /**
         * Creates an AuthenticationResponse message from a plain object. Also converts values to their respective internal types.
         * @function fromObject
         * @memberof nam.AuthenticationResponse
         * @static
         * @param {Object.<string,*>} object Plain object
         * @returns {nam.AuthenticationResponse} AuthenticationResponse
         */
        AuthenticationResponse.fromObject = function fromObject(object) {
            if (object instanceof $root.nam.AuthenticationResponse)
                return object;
            var message = new $root.nam.AuthenticationResponse();
            if (object.success != null)
                message.success = Boolean(object.success);
            if (object.message != null)
                message.message = String(object.message);
            return message;
        };

        /**
         * Creates a plain object from an AuthenticationResponse message. Also converts values to other types if specified.
         * @function toObject
         * @memberof nam.AuthenticationResponse
         * @static
         * @param {nam.AuthenticationResponse} message AuthenticationResponse
         * @param {$protobuf.IConversionOptions} [options] Conversion options
         * @returns {Object.<string,*>} Plain object
         */
        AuthenticationResponse.toObject = function toObject(message, options) {
            if (!options)
                options = {};
            var object = {};
            if (options.defaults) {
                object.success = false;
                object.message = "";
            }
            if (message.success != null && message.hasOwnProperty("success"))
                object.success = message.success;
            if (message.message != null && message.hasOwnProperty("message"))
                object.message = message.message;
            return object;
        };

        /**
         * Converts this AuthenticationResponse to JSON.
         * @function toJSON
         * @memberof nam.AuthenticationResponse
         * @instance
         * @returns {Object.<string,*>} JSON object
         */
        AuthenticationResponse.prototype.toJSON = function toJSON() {
            return this.constructor.toObject(this, $protobuf.util.toJSONOptions);
        };

        return AuthenticationResponse;
    })();

    return nam;
})();

module.exports = $root;
