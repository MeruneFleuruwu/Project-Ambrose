<!-- Project Ambrose by Imjustchico: Spanish review catalog for Ambrose-owned operator and launcher text. -->

# Español (`es`)

This catalog translates text owned by Project Ambrose. It does not translate
Wizard101 client or game text; that content remains in the operator's private
installation and is read at runtime.

## Common interface

| Key | English source | Spanish |
| --- | --- | --- |
| `common.cancel` | Cancel | Cancelar |
| `common.close` | Close | Cerrar |
| `common.copy` | Copy | Copiar |
| `common.retry` | Retry | Reintentar |
| `common.save` | Save | Guardar |
| `common.refresh` | Refresh | Actualizar |
| `common.loading` | Loading | Cargando |
| `common.unavailable` | Unavailable | No disponible |
| `common.unknown` | Unknown | Desconocido |
| `common.required` | Required | Obligatorio |

## Authentication and sessions

| Key | English source | Spanish |
| --- | --- | --- |
| `auth.sign_in` | Sign in | Iniciar sesión |
| `auth.sign_out` | Sign out | Cerrar sesión |
| `auth.password` | Password | Contraseña |
| `auth.invalid_credentials` | The identifier or password is incorrect. | El identificador o la contraseña no son correctos. |
| `auth.second_factor` | Enter your second-factor code. | Introduce tu código del segundo factor. |
| `auth.session_expired` | Your session expired. Sign in again. | Tu sesión ha caducado. Inicia sesión de nuevo. |
| `auth.too_many_attempts` | Too many attempts. Try again later. | Demasiados intentos. Vuelve a intentarlo más tarde. |

## Server operations

| Key | English source | Spanish |
| --- | --- | --- |
| `server.ready` | Ready to accept work. | Listo para aceptar trabajo. |
| `server.starting` | Starting | Iniciando |
| `server.running` | Running | En ejecución |
| `server.stopping` | Stopping | Deteniendo |
| `server.stopped` | Stopped | Detenido |
| `server.degraded` | Degraded | Degradado |
| `server.offline` | Offline | Desconectado |
| `server.maintenance` | Maintenance mode | Modo de mantenimiento |
| `server.restart` | Restart | Reiniciar |
| `server.shutdown` | Shut down | Apagar |
| `server.reload_config` | Reload configuration | Recargar configuración |

## Safe failure messages

| Key | English source | Spanish |
| --- | --- | --- |
| `error.permission_denied` | You do not have permission for this action. | No tienes permiso para esta acción. |
| `error.not_found` | The requested resource was not found. | No se encontró el recurso solicitado. |
| `error.source_unavailable` | This data source is unavailable. | Esta fuente de datos no está disponible. |
| `error.stale_data` | This information may be out of date. | Esta información puede estar desactualizada. |
| `error.operation_failed` | The operation failed. | La operación falló. |
| `error.correlation_id` | Reference: {correlation_id} | Referencia: {correlation_id} |

## Translation notes

- Imperative actions use the informal second-person form consistently in
  messages addressed to the signed-in operator.
- `{correlation_id}` is an opaque value and must remain unchanged.
- Status words are short labels; they are not evidence that an app or realm
  is healthy without the corresponding status data.
- The catalog intentionally contains no client-derived names, game dialogue,
  account identifiers, credentials, packet data, or installation paths.

## Review boundary

This is a wording proposal held for review until the panel, launcher, and
console translation loader defines its final catalog schema. A future loader
must preserve keys, placeholders, and fallback-to-English behavior when a
translation is missing.
