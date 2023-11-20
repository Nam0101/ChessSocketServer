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
            if (message.username != null && Object.hasOwnProperty.call(message, "username"))
                writer.uint32(/* id 1, wireType 2 =*/10).string(message.username);
            if (message.password != null && Object.hasOwnProperty.call(message, "password"))
                writer.uint32(/* id 2, wireType 2 =*/18).string(message.password);
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
                    message.username = reader.string();
                    break;
                case 2:
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
                object.username = "";
                object.password = "";
            }
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
                writer.uint32(/* id 3, wireType 2 =*/26).string(message.username);
            if (message.password != null && Object.hasOwnProperty.call(message, "password"))
                writer.uint32(/* id 4, wireType 2 =*/34).string(message.password);
            if (message.email != null && Object.hasOwnProperty.call(message, "email"))
                writer.uint32(/* id 5, wireType 2 =*/42).string(message.email);
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
                case 3:
                    message.username = reader.string();
                    break;
                case 4:
                    message.password = reader.string();
                    break;
                case 5:
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

    nam.LogoutRequest = (function() {

        /**
         * Properties of a LogoutRequest.
         * @memberof nam
         * @interface ILogoutRequest
         * @property {string|null} [username] LogoutRequest username
         */

        /**
         * Constructs a new LogoutRequest.
         * @memberof nam
         * @classdesc Represents a LogoutRequest.
         * @implements ILogoutRequest
         * @constructor
         * @param {nam.ILogoutRequest=} [properties] Properties to set
         */
        function LogoutRequest(properties) {
            if (properties)
                for (var keys = Object.keys(properties), i = 0; i < keys.length; ++i)
                    if (properties[keys[i]] != null)
                        this[keys[i]] = properties[keys[i]];
        }

        /**
         * LogoutRequest username.
         * @member {string} username
         * @memberof nam.LogoutRequest
         * @instance
         */
        LogoutRequest.prototype.username = "";

        /**
         * Creates a new LogoutRequest instance using the specified properties.
         * @function create
         * @memberof nam.LogoutRequest
         * @static
         * @param {nam.ILogoutRequest=} [properties] Properties to set
         * @returns {nam.LogoutRequest} LogoutRequest instance
         */
        LogoutRequest.create = function create(properties) {
            return new LogoutRequest(properties);
        };

        /**
         * Encodes the specified LogoutRequest message. Does not implicitly {@link nam.LogoutRequest.verify|verify} messages.
         * @function encode
         * @memberof nam.LogoutRequest
         * @static
         * @param {nam.ILogoutRequest} message LogoutRequest message or plain object to encode
         * @param {$protobuf.Writer} [writer] Writer to encode to
         * @returns {$protobuf.Writer} Writer
         */
        LogoutRequest.encode = function encode(message, writer) {
            if (!writer)
                writer = $Writer.create();
            if (message.username != null && Object.hasOwnProperty.call(message, "username"))
                writer.uint32(/* id 6, wireType 2 =*/50).string(message.username);
            return writer;
        };

        /**
         * Encodes the specified LogoutRequest message, length delimited. Does not implicitly {@link nam.LogoutRequest.verify|verify} messages.
         * @function encodeDelimited
         * @memberof nam.LogoutRequest
         * @static
         * @param {nam.ILogoutRequest} message LogoutRequest message or plain object to encode
         * @param {$protobuf.Writer} [writer] Writer to encode to
         * @returns {$protobuf.Writer} Writer
         */
        LogoutRequest.encodeDelimited = function encodeDelimited(message, writer) {
            return this.encode(message, writer).ldelim();
        };

        /**
         * Decodes a LogoutRequest message from the specified reader or buffer.
         * @function decode
         * @memberof nam.LogoutRequest
         * @static
         * @param {$protobuf.Reader|Uint8Array} reader Reader or buffer to decode from
         * @param {number} [length] Message length if known beforehand
         * @returns {nam.LogoutRequest} LogoutRequest
         * @throws {Error} If the payload is not a reader or valid buffer
         * @throws {$protobuf.util.ProtocolError} If required fields are missing
         */
        LogoutRequest.decode = function decode(reader, length) {
            if (!(reader instanceof $Reader))
                reader = $Reader.create(reader);
            var end = length === undefined ? reader.len : reader.pos + length, message = new $root.nam.LogoutRequest();
            while (reader.pos < end) {
                var tag = reader.uint32();
                switch (tag >>> 3) {
                case 6:
                    message.username = reader.string();
                    break;
                default:
                    reader.skipType(tag & 7);
                    break;
                }
            }
            return message;
        };

        /**
         * Decodes a LogoutRequest message from the specified reader or buffer, length delimited.
         * @function decodeDelimited
         * @memberof nam.LogoutRequest
         * @static
         * @param {$protobuf.Reader|Uint8Array} reader Reader or buffer to decode from
         * @returns {nam.LogoutRequest} LogoutRequest
         * @throws {Error} If the payload is not a reader or valid buffer
         * @throws {$protobuf.util.ProtocolError} If required fields are missing
         */
        LogoutRequest.decodeDelimited = function decodeDelimited(reader) {
            if (!(reader instanceof $Reader))
                reader = new $Reader(reader);
            return this.decode(reader, reader.uint32());
        };

        /**
         * Verifies a LogoutRequest message.
         * @function verify
         * @memberof nam.LogoutRequest
         * @static
         * @param {Object.<string,*>} message Plain object to verify
         * @returns {string|null} `null` if valid, otherwise the reason why it is not
         */
        LogoutRequest.verify = function verify(message) {
            if (typeof message !== "object" || message === null)
                return "object expected";
            if (message.username != null && message.hasOwnProperty("username"))
                if (!$util.isString(message.username))
                    return "username: string expected";
            return null;
        };

        /**
         * Creates a LogoutRequest message from a plain object. Also converts values to their respective internal types.
         * @function fromObject
         * @memberof nam.LogoutRequest
         * @static
         * @param {Object.<string,*>} object Plain object
         * @returns {nam.LogoutRequest} LogoutRequest
         */
        LogoutRequest.fromObject = function fromObject(object) {
            if (object instanceof $root.nam.LogoutRequest)
                return object;
            var message = new $root.nam.LogoutRequest();
            if (object.username != null)
                message.username = String(object.username);
            return message;
        };

        /**
         * Creates a plain object from a LogoutRequest message. Also converts values to other types if specified.
         * @function toObject
         * @memberof nam.LogoutRequest
         * @static
         * @param {nam.LogoutRequest} message LogoutRequest
         * @param {$protobuf.IConversionOptions} [options] Conversion options
         * @returns {Object.<string,*>} Plain object
         */
        LogoutRequest.toObject = function toObject(message, options) {
            if (!options)
                options = {};
            var object = {};
            if (options.defaults)
                object.username = "";
            if (message.username != null && message.hasOwnProperty("username"))
                object.username = message.username;
            return object;
        };

        /**
         * Converts this LogoutRequest to JSON.
         * @function toJSON
         * @memberof nam.LogoutRequest
         * @instance
         * @returns {Object.<string,*>} JSON object
         */
        LogoutRequest.prototype.toJSON = function toJSON() {
            return this.constructor.toObject(this, $protobuf.util.toJSONOptions);
        };

        return LogoutRequest;
    })();

    nam.BaseMessage = (function() {

        /**
         * Properties of a BaseMessage.
         * @memberof nam
         * @interface IBaseMessage
         * @property {number|null} [messageType] BaseMessage messageType
         * @property {nam.ILoginRequest|null} [loginRequest] BaseMessage loginRequest
         * @property {nam.ISignupRequest|null} [signupRequest] BaseMessage signupRequest
         * @property {nam.ILogoutRequest|null} [logoutRequest] BaseMessage logoutRequest
         * @property {nam.IAuthenticationResponse|null} [authenticationResponse] BaseMessage authenticationResponse
         */

        /**
         * Constructs a new BaseMessage.
         * @memberof nam
         * @classdesc Represents a BaseMessage.
         * @implements IBaseMessage
         * @constructor
         * @param {nam.IBaseMessage=} [properties] Properties to set
         */
        function BaseMessage(properties) {
            if (properties)
                for (var keys = Object.keys(properties), i = 0; i < keys.length; ++i)
                    if (properties[keys[i]] != null)
                        this[keys[i]] = properties[keys[i]];
        }

        /**
         * BaseMessage messageType.
         * @member {number} messageType
         * @memberof nam.BaseMessage
         * @instance
         */
        BaseMessage.prototype.messageType = 0;

        /**
         * BaseMessage loginRequest.
         * @member {nam.ILoginRequest|null|undefined} loginRequest
         * @memberof nam.BaseMessage
         * @instance
         */
        BaseMessage.prototype.loginRequest = null;

        /**
         * BaseMessage signupRequest.
         * @member {nam.ISignupRequest|null|undefined} signupRequest
         * @memberof nam.BaseMessage
         * @instance
         */
        BaseMessage.prototype.signupRequest = null;

        /**
         * BaseMessage logoutRequest.
         * @member {nam.ILogoutRequest|null|undefined} logoutRequest
         * @memberof nam.BaseMessage
         * @instance
         */
        BaseMessage.prototype.logoutRequest = null;

        /**
         * BaseMessage authenticationResponse.
         * @member {nam.IAuthenticationResponse|null|undefined} authenticationResponse
         * @memberof nam.BaseMessage
         * @instance
         */
        BaseMessage.prototype.authenticationResponse = null;

        /**
         * Creates a new BaseMessage instance using the specified properties.
         * @function create
         * @memberof nam.BaseMessage
         * @static
         * @param {nam.IBaseMessage=} [properties] Properties to set
         * @returns {nam.BaseMessage} BaseMessage instance
         */
        BaseMessage.create = function create(properties) {
            return new BaseMessage(properties);
        };

        /**
         * Encodes the specified BaseMessage message. Does not implicitly {@link nam.BaseMessage.verify|verify} messages.
         * @function encode
         * @memberof nam.BaseMessage
         * @static
         * @param {nam.IBaseMessage} message BaseMessage message or plain object to encode
         * @param {$protobuf.Writer} [writer] Writer to encode to
         * @returns {$protobuf.Writer} Writer
         */
        BaseMessage.encode = function encode(message, writer) {
            if (!writer)
                writer = $Writer.create();
            if (message.messageType != null && Object.hasOwnProperty.call(message, "messageType"))
                writer.uint32(/* id 1, wireType 0 =*/8).int32(message.messageType);
            if (message.loginRequest != null && Object.hasOwnProperty.call(message, "loginRequest"))
                $root.nam.LoginRequest.encode(message.loginRequest, writer.uint32(/* id 2, wireType 2 =*/18).fork()).ldelim();
            if (message.signupRequest != null && Object.hasOwnProperty.call(message, "signupRequest"))
                $root.nam.SignupRequest.encode(message.signupRequest, writer.uint32(/* id 3, wireType 2 =*/26).fork()).ldelim();
            if (message.logoutRequest != null && Object.hasOwnProperty.call(message, "logoutRequest"))
                $root.nam.LogoutRequest.encode(message.logoutRequest, writer.uint32(/* id 4, wireType 2 =*/34).fork()).ldelim();
            if (message.authenticationResponse != null && Object.hasOwnProperty.call(message, "authenticationResponse"))
                $root.nam.AuthenticationResponse.encode(message.authenticationResponse, writer.uint32(/* id 5, wireType 2 =*/42).fork()).ldelim();
            return writer;
        };

        /**
         * Encodes the specified BaseMessage message, length delimited. Does not implicitly {@link nam.BaseMessage.verify|verify} messages.
         * @function encodeDelimited
         * @memberof nam.BaseMessage
         * @static
         * @param {nam.IBaseMessage} message BaseMessage message or plain object to encode
         * @param {$protobuf.Writer} [writer] Writer to encode to
         * @returns {$protobuf.Writer} Writer
         */
        BaseMessage.encodeDelimited = function encodeDelimited(message, writer) {
            return this.encode(message, writer).ldelim();
        };

        /**
         * Decodes a BaseMessage message from the specified reader or buffer.
         * @function decode
         * @memberof nam.BaseMessage
         * @static
         * @param {$protobuf.Reader|Uint8Array} reader Reader or buffer to decode from
         * @param {number} [length] Message length if known beforehand
         * @returns {nam.BaseMessage} BaseMessage
         * @throws {Error} If the payload is not a reader or valid buffer
         * @throws {$protobuf.util.ProtocolError} If required fields are missing
         */
        BaseMessage.decode = function decode(reader, length) {
            if (!(reader instanceof $Reader))
                reader = $Reader.create(reader);
            var end = length === undefined ? reader.len : reader.pos + length, message = new $root.nam.BaseMessage();
            while (reader.pos < end) {
                var tag = reader.uint32();
                switch (tag >>> 3) {
                case 1:
                    message.messageType = reader.int32();
                    break;
                case 2:
                    message.loginRequest = $root.nam.LoginRequest.decode(reader, reader.uint32());
                    break;
                case 3:
                    message.signupRequest = $root.nam.SignupRequest.decode(reader, reader.uint32());
                    break;
                case 4:
                    message.logoutRequest = $root.nam.LogoutRequest.decode(reader, reader.uint32());
                    break;
                case 5:
                    message.authenticationResponse = $root.nam.AuthenticationResponse.decode(reader, reader.uint32());
                    break;
                default:
                    reader.skipType(tag & 7);
                    break;
                }
            }
            return message;
        };

        /**
         * Decodes a BaseMessage message from the specified reader or buffer, length delimited.
         * @function decodeDelimited
         * @memberof nam.BaseMessage
         * @static
         * @param {$protobuf.Reader|Uint8Array} reader Reader or buffer to decode from
         * @returns {nam.BaseMessage} BaseMessage
         * @throws {Error} If the payload is not a reader or valid buffer
         * @throws {$protobuf.util.ProtocolError} If required fields are missing
         */
        BaseMessage.decodeDelimited = function decodeDelimited(reader) {
            if (!(reader instanceof $Reader))
                reader = new $Reader(reader);
            return this.decode(reader, reader.uint32());
        };

        /**
         * Verifies a BaseMessage message.
         * @function verify
         * @memberof nam.BaseMessage
         * @static
         * @param {Object.<string,*>} message Plain object to verify
         * @returns {string|null} `null` if valid, otherwise the reason why it is not
         */
        BaseMessage.verify = function verify(message) {
            if (typeof message !== "object" || message === null)
                return "object expected";
            if (message.messageType != null && message.hasOwnProperty("messageType"))
                if (!$util.isInteger(message.messageType))
                    return "messageType: integer expected";
            if (message.loginRequest != null && message.hasOwnProperty("loginRequest")) {
                var error = $root.nam.LoginRequest.verify(message.loginRequest);
                if (error)
                    return "loginRequest." + error;
            }
            if (message.signupRequest != null && message.hasOwnProperty("signupRequest")) {
                var error = $root.nam.SignupRequest.verify(message.signupRequest);
                if (error)
                    return "signupRequest." + error;
            }
            if (message.logoutRequest != null && message.hasOwnProperty("logoutRequest")) {
                var error = $root.nam.LogoutRequest.verify(message.logoutRequest);
                if (error)
                    return "logoutRequest." + error;
            }
            if (message.authenticationResponse != null && message.hasOwnProperty("authenticationResponse")) {
                var error = $root.nam.AuthenticationResponse.verify(message.authenticationResponse);
                if (error)
                    return "authenticationResponse." + error;
            }
            return null;
        };

        /**
         * Creates a BaseMessage message from a plain object. Also converts values to their respective internal types.
         * @function fromObject
         * @memberof nam.BaseMessage
         * @static
         * @param {Object.<string,*>} object Plain object
         * @returns {nam.BaseMessage} BaseMessage
         */
        BaseMessage.fromObject = function fromObject(object) {
            if (object instanceof $root.nam.BaseMessage)
                return object;
            var message = new $root.nam.BaseMessage();
            if (object.messageType != null)
                message.messageType = object.messageType | 0;
            if (object.loginRequest != null) {
                if (typeof object.loginRequest !== "object")
                    throw TypeError(".nam.BaseMessage.loginRequest: object expected");
                message.loginRequest = $root.nam.LoginRequest.fromObject(object.loginRequest);
            }
            if (object.signupRequest != null) {
                if (typeof object.signupRequest !== "object")
                    throw TypeError(".nam.BaseMessage.signupRequest: object expected");
                message.signupRequest = $root.nam.SignupRequest.fromObject(object.signupRequest);
            }
            if (object.logoutRequest != null) {
                if (typeof object.logoutRequest !== "object")
                    throw TypeError(".nam.BaseMessage.logoutRequest: object expected");
                message.logoutRequest = $root.nam.LogoutRequest.fromObject(object.logoutRequest);
            }
            if (object.authenticationResponse != null) {
                if (typeof object.authenticationResponse !== "object")
                    throw TypeError(".nam.BaseMessage.authenticationResponse: object expected");
                message.authenticationResponse = $root.nam.AuthenticationResponse.fromObject(object.authenticationResponse);
            }
            return message;
        };

        /**
         * Creates a plain object from a BaseMessage message. Also converts values to other types if specified.
         * @function toObject
         * @memberof nam.BaseMessage
         * @static
         * @param {nam.BaseMessage} message BaseMessage
         * @param {$protobuf.IConversionOptions} [options] Conversion options
         * @returns {Object.<string,*>} Plain object
         */
        BaseMessage.toObject = function toObject(message, options) {
            if (!options)
                options = {};
            var object = {};
            if (options.defaults) {
                object.messageType = 0;
                object.loginRequest = null;
                object.signupRequest = null;
                object.logoutRequest = null;
                object.authenticationResponse = null;
            }
            if (message.messageType != null && message.hasOwnProperty("messageType"))
                object.messageType = message.messageType;
            if (message.loginRequest != null && message.hasOwnProperty("loginRequest"))
                object.loginRequest = $root.nam.LoginRequest.toObject(message.loginRequest, options);
            if (message.signupRequest != null && message.hasOwnProperty("signupRequest"))
                object.signupRequest = $root.nam.SignupRequest.toObject(message.signupRequest, options);
            if (message.logoutRequest != null && message.hasOwnProperty("logoutRequest"))
                object.logoutRequest = $root.nam.LogoutRequest.toObject(message.logoutRequest, options);
            if (message.authenticationResponse != null && message.hasOwnProperty("authenticationResponse"))
                object.authenticationResponse = $root.nam.AuthenticationResponse.toObject(message.authenticationResponse, options);
            return object;
        };

        /**
         * Converts this BaseMessage to JSON.
         * @function toJSON
         * @memberof nam.BaseMessage
         * @instance
         * @returns {Object.<string,*>} JSON object
         */
        BaseMessage.prototype.toJSON = function toJSON() {
            return this.constructor.toObject(this, $protobuf.util.toJSONOptions);
        };

        return BaseMessage;
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
